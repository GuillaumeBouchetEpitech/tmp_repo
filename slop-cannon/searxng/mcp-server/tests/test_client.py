import httpx
import pytest
import respx

from searxng_mcp.client import SearxngClient, SearxngError, format_markdown

BASE = "http://searxng.test"

SAMPLE = {
    "query": "python",
    "results": [
        {"title": "Python", "url": "https://python.org", "content": "Official   site\n", "engines": ["duckduckgo", "bing"]},
        {"title": "Dup", "url": "https://python.org", "content": "duplicate", "engine": "brave"},
        {"title": "Wiki", "url": "https://en.wikipedia.org/wiki/Python", "content": "x" * 500, "engine": "wikipedia",
         "publishedDate": "2024-05-01T00:00:00"},
        {"title": "Third", "url": "https://example.com", "content": ""},
    ],
    "answers": [{"answer": "42"}],
    "infoboxes": [{"infobox": "Python", "content": "A language", "urls": [{"url": "https://python.org"}]}],
    "suggestions": ["python tutorial"],
    "unresponsive_engines": [["google", "timeout"]],
}


@respx.mock
async def test_search_parses_and_dedupes():
    route = respx.get(f"{BASE}/search").mock(return_value=httpx.Response(200, json=SAMPLE))
    c = SearxngClient(BASE)
    resp = await c.search("python", engines=["duckduckgo", "bing"], time_range="week", max_results=2, snippet_chars=50)

    params = route.calls.last.request.url.params
    assert params["format"] == "json"
    assert params["engines"] == "duckduckgo,bing"
    assert params["time_range"] == "week"

    assert [r.url for r in resp.results] == ["https://python.org", "https://en.wikipedia.org/wiki/Python"]
    assert resp.results[0].content == "Official site"
    assert len(resp.results[1].content) == 50
    assert resp.results[1].engines == ["wikipedia"]
    assert resp.answers == ["42"]
    assert resp.unresponsive_engines == ["google (timeout)"]

    md = format_markdown(resp)
    assert "[Python](https://python.org)" in md
    assert "**Answer:** 42" in md
    assert "google (timeout)" in md


@respx.mock
@pytest.mark.parametrize("status,msg", [(403, "json"), (429, "rate limited"), (500, "HTTP 500")])
async def test_http_errors(status, msg):
    respx.get(f"{BASE}/search").mock(return_value=httpx.Response(status))
    with pytest.raises(SearxngError, match=msg):
        await SearxngClient(BASE).search("q")


@respx.mock
async def test_connection_error():
    respx.get(f"{BASE}/search").mock(side_effect=httpx.ConnectError("boom"))
    with pytest.raises(SearxngError, match="Could not reach"):
        await SearxngClient(BASE).search("q")

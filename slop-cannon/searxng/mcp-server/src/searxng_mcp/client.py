"""Async client for the SearXNG JSON API."""

from dataclasses import dataclass, field
from typing import Any

import httpx


class SearxngError(Exception):
    pass


@dataclass
class SearchResult:
    title: str
    url: str
    content: str
    engines: list[str]
    published: str | None = None


@dataclass
class SearchResponse:
    query: str
    results: list[SearchResult]
    answers: list[str] = field(default_factory=list)
    infoboxes: list[dict[str, str]] = field(default_factory=list)
    suggestions: list[str] = field(default_factory=list)
    unresponsive_engines: list[str] = field(default_factory=list)


def _truncate(text: str, limit: int) -> str:
    text = " ".join(text.split())
    return text if len(text) <= limit else text[: limit - 1].rstrip() + "…"


class SearxngClient:
    def __init__(self, base_url: str, timeout: float = 15.0, http: httpx.AsyncClient | None = None):
        self._base_url = base_url.rstrip("/")
        self._http = http or httpx.AsyncClient(
            timeout=timeout,
            transport=httpx.AsyncHTTPTransport(retries=1),
        )

    async def aclose(self) -> None:
        await self._http.aclose()

    async def search(
        self,
        query: str,
        *,
        categories: list[str] | None = None,
        engines: list[str] | None = None,
        language: str | None = None,
        time_range: str | None = None,
        safesearch: int | None = None,
        page: int = 1,
        max_results: int = 10,
        snippet_chars: int = 300,
    ) -> SearchResponse:
        params: dict[str, Any] = {"q": query, "format": "json", "pageno": page}
        if categories:
            params["categories"] = ",".join(categories)
        if engines:
            params["engines"] = ",".join(engines)
        if language:
            params["language"] = language
        if time_range:
            params["time_range"] = time_range
        if safesearch is not None:
            params["safesearch"] = safesearch

        try:
            resp = await self._http.get(f"{self._base_url}/search", params=params)
        except httpx.HTTPError as e:
            raise SearxngError(f"Could not reach SearXNG at {self._base_url}: {e}") from e
        if resp.status_code == 403:
            raise SearxngError("SearXNG returned 403: is 'json' enabled in search.formats?")
        if resp.status_code == 429:
            raise SearxngError("SearXNG returned 429: rate limited (is the limiter enabled?)")
        if resp.status_code != 200:
            raise SearxngError(f"SearXNG returned HTTP {resp.status_code}")
        try:
            data = resp.json()
        except ValueError as e:
            raise SearxngError("SearXNG returned invalid JSON") from e

        return self._parse(query, data, max_results, snippet_chars)

    @staticmethod
    def _parse(query: str, data: dict[str, Any], max_results: int, snippet_chars: int) -> SearchResponse:
        seen: set[str] = set()
        results: list[SearchResult] = []
        for r in data.get("results", []):
            url = r.get("url")
            if not url or url in seen:
                continue
            seen.add(url)
            results.append(
                SearchResult(
                    title=_truncate(r.get("title") or url, 200),
                    url=url,
                    content=_truncate(r.get("content") or "", snippet_chars),
                    engines=r.get("engines") or ([r["engine"]] if r.get("engine") else []),
                    published=r.get("publishedDate"),
                )
            )
            if len(results) >= max_results:
                break

        answers = []
        for a in data.get("answers", []):
            text = a.get("answer") if isinstance(a, dict) else a
            if text:
                answers.append(_truncate(str(text), 1000))

        infoboxes = [
            {
                "title": ib.get("infobox", ""),
                "content": _truncate(ib.get("content") or "", 1000),
                "url": (ib.get("urls") or [{}])[0].get("url", "") if ib.get("urls") else ib.get("id", ""),
            }
            for ib in data.get("infoboxes", [])
        ]

        unresponsive = [
            f"{e[0]} ({e[1]})" if isinstance(e, (list, tuple)) and len(e) > 1 else str(e)
            for e in data.get("unresponsive_engines", [])
        ]

        return SearchResponse(
            query=query,
            results=results,
            answers=answers,
            infoboxes=infoboxes,
            suggestions=list(data.get("suggestions", []))[:5],
            unresponsive_engines=unresponsive,
        )


def format_markdown(resp: SearchResponse, page: int = 1) -> str:
    lines = [f"# Search: {resp.query}" + (f" (page {page})" if page > 1 else ""), ""]
    for a in resp.answers:
        lines += [f"**Answer:** {a}", ""]
    for ib in resp.infoboxes:
        lines += [f"**{ib['title']}**: {ib['content']}" + (f" ({ib['url']})" if ib["url"] else ""), ""]
    if not resp.results:
        lines.append("No results.")
    for i, r in enumerate(resp.results, 1):
        meta = ", ".join(r.engines)
        if r.published:
            meta += f"; published {r.published[:10]}"
        lines.append(f"{i}. [{r.title}]({r.url})  ({meta})")
        if r.content:
            lines.append(f"   {r.content}")
    if resp.suggestions:
        lines += ["", "Related searches: " + "; ".join(resp.suggestions)]
    if resp.unresponsive_engines:
        lines += ["", "Engines that failed: " + ", ".join(resp.unresponsive_engines)]
    return "\n".join(lines)

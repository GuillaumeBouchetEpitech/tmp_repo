"""MCP server exposing SearXNG web search."""

import argparse
import os
from typing import Literal

from mcp.server.mcpserver import MCPServer
from mcp.server.transport_security import TransportSecuritySettings
from starlette.requests import Request
from starlette.responses import JSONResponse

from .client import SearxngClient, SearxngError, format_markdown
from .config import Config

config = Config.from_env()
client = SearxngClient(config.searxng_url, timeout=config.timeout)

mcp = MCPServer(
    name="searxng",
    instructions="Web search through a private SearXNG metasearch instance. "
    "Use web_search for current information, documentation, news and anything outside your training data.",
)


@mcp.tool()
async def web_search(
    query: str,
    categories: list[str] | None = None,
    engines: list[str] | None = None,
    language: str | None = None,
    time_range: Literal["day", "week", "month", "year"] | None = None,
    safesearch: Literal[0, 1, 2] | None = None,
    page: int = 1,
    max_results: int | None = None,
) -> str:
    """Search the web with SearXNG and return ranked results as Markdown.

    Args:
        query: Search query. SearXNG syntax like `!wp` (engine bang) or `:fr` (language) is supported.
        categories: e.g. ["general"], ["news"], ["it"], ["science"], ["images"], ["videos"].
        engines: Restrict to specific engines, e.g. ["duckduckgo", "wikipedia", "github"].
        language: Language code such as "en", "fr", "de-DE". Defaults to the instance setting.
        time_range: Only return results from the last day/week/month/year.
        safesearch: 0 = off, 1 = moderate, 2 = strict.
        page: Result page, starting at 1.
        max_results: Maximum number of results to return (1-50, default 10).
    """
    if not query.strip():
        return "Error: query must not be empty."
    limit = max(1, min(max_results or config.default_max_results, 50))
    try:
        resp = await client.search(
            query,
            categories=categories,
            engines=engines,
            language=language,
            time_range=time_range,
            safesearch=safesearch,
            page=max(1, page),
            max_results=limit,
            snippet_chars=config.snippet_chars,
        )
    except SearxngError as e:
        return f"Error: {e}"
    return format_markdown(resp, page=max(1, page))


@mcp.custom_route("/health", methods=["GET"])
async def health(_: Request) -> JSONResponse:
    return JSONResponse({"status": "ok"})


def main() -> None:
    parser = argparse.ArgumentParser(description="SearXNG MCP server")
    parser.add_argument("--transport", choices=["stdio", "streamable-http"], default=os.environ.get("MCP_TRANSPORT", "stdio"))
    parser.add_argument("--host", default=os.environ.get("MCP_HOST", "127.0.0.1"))
    parser.add_argument("--port", type=int, default=int(os.environ.get("MCP_PORT", "8000")))
    args = parser.parse_args()

    if args.transport == "stdio":
        mcp.run("stdio")
        return

    # Inside Docker we bind 0.0.0.0, so DNS-rebinding protection must be configured
    # explicitly with the Host/Origin values clients will actually send.
    allowed_hosts = os.environ.get("MCP_ALLOWED_HOSTS", "127.0.0.1:*,localhost:*").split(",")
    allowed_origins = os.environ.get(
        "MCP_ALLOWED_ORIGINS", "http://127.0.0.1:*,http://localhost:*"
    ).split(",")
    mcp.run(
        "streamable-http",
        host=args.host,
        port=args.port,
        transport_security=TransportSecuritySettings(
            enable_dns_rebinding_protection=True,
            allowed_hosts=[h.strip() for h in allowed_hosts if h.strip()],
            allowed_origins=[o.strip() for o in allowed_origins if o.strip()],
        ),
    )


if __name__ == "__main__":
    main()

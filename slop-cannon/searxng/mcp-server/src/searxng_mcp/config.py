import os
from dataclasses import dataclass


@dataclass(frozen=True)
class Config:
    searxng_url: str
    timeout: float
    default_max_results: int
    snippet_chars: int

    @classmethod
    def from_env(cls) -> "Config":
        return cls(
            searxng_url=os.environ.get("SEARXNG_URL", "http://localhost:8080").rstrip("/"),
            timeout=float(os.environ.get("SEARXNG_TIMEOUT", "15")),
            default_max_results=int(os.environ.get("MAX_RESULTS", "10")),
            snippet_chars=int(os.environ.get("SNIPPET_CHARS", "300")),
        )

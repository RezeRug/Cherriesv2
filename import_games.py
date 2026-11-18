#!/usr/bin/env python3
"""Validate and merge a CSV game list into data/games.json.

CSV columns: id,title,category,description,url,symbol
Only HTTPS links are accepted. This tool does not test whether a school network
allows a site; it checks formatting and catalog data only.
"""
import csv
import json
import sys
from pathlib import Path
from urllib.parse import urlparse

ROOT = Path(__file__).resolve().parents[1]
CATALOG = ROOT / "data" / "games.json"
ALLOWED_CATEGORIES = {"Arcade", "Puzzle", "Strategy", "Casual"}

def main():
    if len(sys.argv) != 2:
        raise SystemExit("Usage: python tools/import_games.py games.csv")
    source = Path(sys.argv[1])
    if not source.is_file():
        raise SystemExit(f"CSV not found: {source}")

    current = json.loads(CATALOG.read_text(encoding="utf-8"))
    by_id = {game["id"]: game for game in current}
    added = 0
    with source.open(newline="", encoding="utf-8-sig") as handle:
        reader = csv.DictReader(handle)
        required = {"id", "title", "category", "description", "url", "symbol"}
        if not required.issubset(reader.fieldnames or []):
            raise SystemExit(f"CSV must contain columns: {', '.join(sorted(required))}")

        for line, row in enumerate(reader, start=2):
            game_id = row["id"].strip()
            title = row["title"].strip()
            category = row["category"].strip()
            url = row["url"].strip()
            parsed = urlparse(url)

            if not game_id or not title:
                raise SystemExit(f"Line {line}: id and title are required")
            if category not in ALLOWED_CATEGORIES:
                raise SystemExit(f"Line {line}: category must be one of {sorted(ALLOWED_CATEGORIES)}")
            if parsed.scheme != "https" or not parsed.netloc:
                raise SystemExit(f"Line {line}: only valid HTTPS URLs are accepted")

            by_id[game_id] = {
                "id": game_id,
                "title": title,
                "category": category,
                "description": row["description"].strip(),
                "url": url,
                "symbol": row["symbol"].strip() or "✦",
            }
            added += 1

    CATALOG.write_text(json.dumps(list(by_id.values()), indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"Imported/updated {added} rows. Catalog now contains {len(by_id)} entries.")
    print("URL format was checked; actual reachability and school-network access were not tested.")

if __name__ == "__main__":
    main()

# Cherries

Cherries is a responsive browser-game directory with a C++ backend. It lists games from a local JSON catalog and opens each game's official HTTPS page in a new tab.

> This is a game directory, not a network-filter bypass. School networks may block game sites, and the project does not attempt to evade those restrictions. Add only games that you are permitted to access and link to.

## Features

- C++17 backend using Crow
- JSON game catalog
- Search by title, description, or category
- Category filters
- Favorites saved in the browser
- Responsive desktop and mobile layout
- Safe external links (`https` only, `noopener noreferrer`)
- CSV importer for adding large catalogs

## Project structure

```text
cherries/
├── CMakeLists.txt
├── README.md
├── games-template.csv
├── data/
│   └── games.json
├── public/
│   ├── app.js
│   ├── favicon.svg
│   ├── index.html
│   └── styles.css
├── src/
│   └── main.cpp
└── tools/
    └── import_games.py
```

## Requirements

- CMake 3.20+
- A C++17 compiler (GCC, Clang, or MSVC)
- Git
- Internet access during the first CMake configure/build so CMake can download Crow and nlohmann/json

## Build and run

### macOS / Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/cherries
```

### Windows (PowerShell, Visual Studio Build Tools)

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
.\build\Release\cherries.exe
```

Open `http://localhost:8080` in your browser.

To use a different port:

macOS/Linux:
```bash
PORT=8081 ./build/cherries
```

PowerShell:
```powershell
$env:PORT="8081"
.\build\Release\cherries.exe
```

The server binds to `127.0.0.1` by default, so it is local to your computer.

## Add games — including 500+

1. Edit `data/games.json`, or copy `games-template.csv` and add rows.
2. Use a unique `id` for each game.
3. Use one of these categories: `Arcade`, `Puzzle`, `Strategy`, `Casual`.
4. Use an official, permitted HTTPS game URL.
5. Import a CSV:

```bash
python tools/import_games.py my-games.csv
```

6. Restart the server or refresh the page.

The catalog has no 500-game limit. The starter catalog contains a small set of example destinations; it does **not** claim that 500 school-accessible games have been verified. The importer reports how many entries are in the catalog after import.

## API

- `GET /api/health` — health check
- `GET /api/games` — full catalog
- `GET /api/games?category=Puzzle` — category filter
- `GET /api/games?q=snake` — text search

## Important notes

- A valid HTTPS URL does not guarantee that a game is online, embeddable, appropriate for school, or allowed by a school network.
- Cherries opens external game pages rather than embedding them. This avoids breaking sites that prohibit framing.
- Check each game's terms, age rating, licensing, and school rules before adding it.
- The starter catalog's links are examples and should be tested on your intended device and network.
- For a public deployment, add HTTPS hosting, rate limiting, monitoring, and a deployment-specific security review. Do not expose a development server directly to the public internet.

const state = { games: [], category: "All", query: "", favoritesOnly: false };
const $ = (selector) => document.querySelector(selector);
const template = $("#game-card-template");

function getFavorites() {
  try { return new Set(JSON.parse(localStorage.getItem("cherries-favorites") || "[]")); }
  catch { return new Set(); }
}
function saveFavorites(favorites) {
  localStorage.setItem("cherries-favorites", JSON.stringify([...favorites]));
}
function safeExternalUrl(value) {
  try {
    const url = new URL(value);
    return url.protocol === "https:" ? url.href : null;
  } catch { return null; }
}
function render() {
  const favorites = getFavorites();
  const filtered = state.games.filter((game) => {
    const matchesCategory = state.category === "All" || game.category === state.category;
    const searchText = `${game.title} ${game.description} ${game.category}`.toLowerCase();
    const matchesQuery = searchText.includes(state.query.toLowerCase());
    const matchesFavorite = !state.favoritesOnly || favorites.has(game.id);
    return matchesCategory && matchesQuery && matchesFavorite;
  });

  $("#games").replaceChildren();
  $("#empty-state").hidden = filtered.length !== 0;

  for (const game of filtered) {
    const url = safeExternalUrl(game.url);
    if (!url) continue;
    const card = template.content.firstElementChild.cloneNode(true);
    card.querySelector(".art-symbol").textContent = game.symbol || "✦";
    card.querySelector(".game-category").textContent = game.category;
    card.querySelector(".game-title").textContent = game.title;
    card.querySelector(".game-description").textContent = game.description;
    const link = card.querySelector(".play-button");
    link.href = url;
    link.setAttribute("aria-label", `Open ${game.title} on its official website`);

    const favoriteButton = card.querySelector(".favorite-button");
    const saved = favorites.has(game.id);
    favoriteButton.classList.toggle("saved", saved);
    favoriteButton.textContent = saved ? "♥" : "♡";
    favoriteButton.setAttribute("aria-label", saved ? "Remove from favorites" : "Add to favorites");
    favoriteButton.addEventListener("click", () => {
      const next = getFavorites();
      next.has(game.id) ? next.delete(game.id) : next.add(game.id);
      saveFavorites(next);
      render();
    });
    $("#games").append(card);
  }
  $("#game-count").textContent = `${state.games.length} games in the directory`;
}

async function init() {
  $("#year").textContent = new Date().getFullYear();
  try {
    const response = await fetch("/api/games");
    if (!response.ok) throw new Error("Catalog request failed");
    state.games = await response.json();
    render();
  } catch (error) {
    $("#games").innerHTML = '<div class="loading-card">Could not load the game library. Check that the Cherries server is running.</div>';
    $("#game-count").textContent = "Library unavailable";
    console.error(error);
  }
}

$("#search").addEventListener("input", (event) => {
  state.query = event.target.value.trim();
  render();
});
$("#categories").addEventListener("click", (event) => {
  const button = event.target.closest("[data-category]");
  if (!button) return;
  state.category = button.dataset.category;
  document.querySelectorAll(".category").forEach((item) => item.classList.toggle("active", item === button));
  render();
});
$("#favorites-toggle").addEventListener("click", () => {
  state.favoritesOnly = !state.favoritesOnly;
  $("#favorites-toggle").classList.toggle("active", state.favoritesOnly);
  $("#favorites-toggle").setAttribute("aria-pressed", String(state.favoritesOnly));
  $("#favorites-toggle").textContent = state.favoritesOnly ? "♥ Favorites" : "♡ Favorites";
  render();
});
document.addEventListener("keydown", (event) => {
  if (event.key === "/" && !["INPUT", "TEXTAREA"].includes(document.activeElement.tagName)) {
    event.preventDefault();
    $("#search").focus();
  }
});
init();

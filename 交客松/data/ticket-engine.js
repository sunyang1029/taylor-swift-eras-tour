/**
 *  Ticket Engine — Countdown + Dynamic Price Simulator
 *  核心票务引擎：倒计时 + 动态价格波动模拟
 */
import { readFileSync } from "fs";
import { join, dirname } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const DATA_PATH = join(__dirname, "tour-data.json");

// ── Data Loader ───────────────────────────────────────────
function loadData() {
  const raw = readFileSync(DATA_PATH, "utf-8");
  return JSON.parse(raw);
}

// ── Helpers ───────────────────────────────────────────────
function getAllConcerts(data) {
  const concerts = [];
  for (const [artistId, artist] of Object.entries(data.artists)) {
    for (const c of artist.concerts) {
      concerts.push({ ...c, artistId, artistName: artist.name, artistNameZh: artist.nameZh });
    }
  }
  return concerts;
}

function parseDate(dateStr) {
  return new Date(dateStr);
}

// ──────────────────────────────────────────────────────────
//  COUNTDOWN ENGINE
// ──────────────────────────────────────────────────────────

/**
 * Find the nearest upcoming concert (overall or per artist).
 * Returns null if no future concerts exist.
 */
export function findNextConcert(artistId = null) {
  const data = loadData();
  const now = Date.now();

  let candidates;
  if (artistId && data.artists[artistId]) {
    candidates = data.artists[artistId].concerts.map((c) => ({
      ...c,
      artistId,
      artistName: data.artists[artistId].name,
      artistNameZh: data.artists[artistId].nameZh,
    }));
  } else {
    candidates = getAllConcerts(data);
  }

  const upcoming = candidates
    .filter((c) => parseDate(c.date).getTime() > now)
    .sort((a, b) => parseDate(a.date).getTime() - parseDate(b.date).getTime());

  return upcoming.length > 0 ? upcoming[0] : null;
}

/**
 * Get detailed countdown for a specific concert or the next one.
 */
export function getCountdown(concertId = null) {
  const data = loadData();
  const now = Date.now();

  let concert;
  if (concertId) {
    concert = getAllConcerts(data).find((c) => c.id === concertId);
    if (!concert) return { error: "Concert not found", concertId };
  } else {
    concert = findNextConcert();
    if (!concert) return { error: "No upcoming concerts", concertId: null };
  }

  const target = parseDate(concert.date).getTime();
  const diff = target - now;

  if (diff <= 0) {
    return {
      concert,
      status: "live",
      message: concert.status === "sold-out" ? "演唱会进行中 — 已售罄" : "演唱会正在进行！",
      diffMs: 0,
      days: 0,
      hours: 0,
      minutes: 0,
      seconds: 0,
      isUpcoming: false,
    };
  }

  const days = Math.floor(diff / 86400000);
  const hours = Math.floor((diff % 86400000) / 3600000);
  const minutes = Math.floor((diff % 3600000) / 60000);
  const seconds = Math.floor((diff % 60000) / 1000);

  return {
    concert,
    status: "upcoming",
    diffMs: diff,
    days,
    hours,
    minutes,
    seconds,
    totalHours: Math.floor(diff / 3600000),
    totalMinutes: Math.floor(diff / 60000),
    iso: concert.date,
    isUpcoming: true,
  };
}

/**
 * Get countdowns for all upcoming concerts (for the dashboard list).
 */
export function getAllCountdowns(artistId = null) {
  const data = loadData();
  const now = Date.now();

  let concerts = artistId
    ? (data.artists[artistId]?.concerts || []).map((c) => ({
        ...c,
        artistId,
        artistName: data.artists[artistId].name,
        artistNameZh: data.artists[artistId].nameZh,
      }))
    : getAllConcerts(data);

  return concerts
    .filter((c) => parseDate(c.date).getTime() > now)
    .map((c) => {
      const diff = parseDate(c.date).getTime() - now;
      const days = Math.floor(diff / 86400000);
      const hours = Math.floor((diff % 86400000) / 3600000);
      return {
        concertId: c.id,
        city: c.city,
        cityZh: c.cityZh,
        venue: c.venue,
        date: c.date,
        artistId: c.artistId,
        artistName: c.artistName,
        diffMs: diff,
        days,
        hours,
        lat: c.lat,
        lng: c.lng,
        status: c.status,
      };
    })
    .sort((a, b) => a.diffMs - b.diffMs);
}

// ──────────────────────────────────────────────────────────
//  DYNAMIC PRICE SIMULATOR
// ──────────────────────────────────────────────────────────

const PRICE_TIERS = [
  { min: 0, max: 0.5, label: "normal", labelZh: "正常", multiplier: 1.0, color: "#22c55e" },
  { min: 0.5, max: 0.7, label: "warming", labelZh: "升温", multiplier: 1.3, color: "#eab308" },
  { min: 0.7, max: 0.85, label: "hot", labelZh: "火爆", multiplier: 1.8, color: "#f97316" },
  { min: 0.85, max: 0.95, label: "critical", labelZh: "告急", multiplier: 2.5, color: "#ef4444" },
  { min: 0.95, max: 0.99, label: "extreme", labelZh: "极度紧缺", multiplier: 4.0, color: "#dc2626" },
  { min: 0.99, max: 1.01, label: "sold-out", labelZh: "售罄", multiplier: 6.0, color: "#7c3aed" },
];

/**
 * Determine price tier and multiplier based on sell-out rate.
 */
function getPriceTier(sellOutRate) {
  return PRICE_TIERS.find((t) => sellOutRate >= t.min && sellOutRate < t.max) || PRICE_TIERS[PRICE_TIERS.length - 1];
}

/**
 * Simulate a "live" price update with random fluctuation.
 * Called per-request to make prices feel real and dynamic.
 */
export function getPriceStatus(concertId) {
  const data = loadData();
  const concert = getAllConcerts(data).find((c) => c.id === concertId);
  if (!concert) return { error: "Concert not found", concertId };

  const sellOutRate = concert.ticketsSold / concert.capacity;
  const tier = getPriceTier(sellOutRate);

  // ±8% random fluctuation for live feel
  const noise = 0.92 + Math.random() * 0.16;
  const dynamicMultiplier = +(tier.multiplier * noise).toFixed(2);

  // Trend direction: random walk bias based on sellout momentum
  const trendRoll = Math.random();
  let trend;
  if (sellOutRate > 0.9) trend = "volatile";
  else if (sellOutRate > 0.7 && trendRoll > 0.5) trend = "rising";
  else if (sellOutRate < 0.4 && trendRoll > 0.6) trend = "stable";
  else trend = trendRoll > 0.3 ? "rising" : "stable";

  const currentPrice = +(concert.basePrice * dynamicMultiplier).toFixed(0);
  const faceValue = concert.basePrice;
  const premium = +((dynamicMultiplier - 1) * 100).toFixed(0);

  return {
    concertId,
    city: concert.city,
    cityZh: concert.cityZh,
    artistName: concert.artistName,
    artistNameZh: concert.artistNameZh,
    venue: concert.venue,
    date: concert.date,
    capacity: concert.capacity,
    ticketsSold: concert.ticketsSold,
    ticketsRemaining: concert.capacity - concert.ticketsSold,
    sellOutRate: +(sellOutRate * 100).toFixed(1),
    faceValue,
    currency: concert.currency,
    currentPrice,
    multiplier: dynamicMultiplier,
    premium,
    tier: tier.label,
    tierZh: tier.labelZh,
    tierColor: tier.color,
    trend,
    trendEmoji: { rising: "📈", stable: "📊", volatile: "🎢" }[trend],
    simulatedAt: new Date().toISOString(),
  };
}

/**
 * Batch price status for all concerts (for map markers).
 */
export function getAllPriceStatuses(artistId = null) {
  const data = loadData();

  let concerts;
  if (artistId && data.artists[artistId]) {
    concerts = data.artists[artistId].concerts.map((c) => ({
      ...c,
      artistId,
      artistName: data.artists[artistId].name,
      artistNameZh: data.artists[artistId].nameZh,
    }));
  } else {
    concerts = getAllConcerts(data);
  }

  return concerts.map((c) => {
    const sellOutRate = c.ticketsSold / c.capacity;
    const tier = getPriceTier(sellOutRate);
    const noise = 0.92 + Math.random() * 0.16;
    const dynamicMultiplier = +(tier.multiplier * noise).toFixed(2);
    const currentPrice = +(c.basePrice * dynamicMultiplier).toFixed(0);

    return {
      concertId: c.id,
      city: c.city,
      cityZh: c.cityZh,
      artistId: c.artistId,
      artistName: c.artistName,
      lat: c.lat,
      lng: c.lng,
      venue: c.venue,
      date: c.date,
      capacity: c.capacity,
      ticketsSold: c.ticketsSold,
      ticketsRemaining: c.capacity - c.ticketsSold,
      sellOutRate: +(sellOutRate * 100).toFixed(1),
      faceValue: c.basePrice,
      currency: c.currency,
      currentPrice,
      multiplier: dynamicMultiplier,
      tier: tier.label,
      tierZh: tier.labelZh,
      tierColor: tier.color,
      status: c.status,
    };
  });
}

/**
 * Simulate "ticket purchased" event — increment sold count.
 * Makes the data feel alive by simulating real-time buying.
 */
export function simulatePurchase(concertId, quantity = 1) {
  const data = loadData();
  const concert = getAllConcerts(data).find((c) => c.id === concertId);
  if (!concert) return { error: "Concert not found", concertId };

  const newSold = Math.min(concert.ticketsSold + quantity, concert.capacity);
  const wasSoldOut = concert.ticketsSold >= concert.capacity;
  concert.ticketsSold = newSold;

  if (newSold >= concert.capacity && !wasSoldOut) {
    concert.status = "sold-out";
  }

  // Note: This only mutates in-memory — doesn't persist to file.
  // For hackathon MVP this is fine; each server restart resets.
  return {
    concertId,
    ticketsSold: newSold,
    ticketsRemaining: concert.capacity - newSold,
    status: concert.status,
    sellOutRate: +((newSold / concert.capacity) * 100).toFixed(1),
  };
}

/**
 * Dashboard summary — key metrics for the front page.
 */
export function getDashboardSummary() {
  const data = loadData();
  const all = getAllConcerts(data);
  const now = Date.now();
  const upcoming = all.filter((c) => parseDate(c.date).getTime() > now);

  const totalConcerts = upcoming.length;
  const totalCapacity = upcoming.reduce((s, c) => s + c.capacity, 0);
  const totalSold = upcoming.reduce((s, c) => s + c.ticketsSold, 0);
  const overallSellOut = totalCapacity > 0 ? +((totalSold / totalCapacity) * 100).toFixed(1) : 0;

  const byArtist = {};
  for (const [id, artist] of Object.entries(data.artists)) {
    const artistUpcoming = artist.concerts.filter((c) => parseDate(c.date).getTime() > now);
    byArtist[id] = {
      name: artist.name,
      nameZh: artist.nameZh,
      tourName: artist.tourName,
      concertCount: artistUpcoming.length,
      totalSold: artistUpcoming.reduce((s, c) => s + c.ticketsSold, 0),
      avgSellOutRate: artistUpcoming.length
        ? +((artistUpcoming.reduce((s, c) => s + c.ticketsSold / c.capacity, 0) / artistUpcoming.length) * 100).toFixed(1)
        : 0,
    };
  }

  const next = findNextConcert();
  const soldOutCount = upcoming.filter((c) => c.status === "sold-out").length;

  return {
    totalConcerts,
    totalCapacity,
    totalSold,
    overallSellOut,
    soldOutCount,
    nextConcert: next
      ? {
          concertId: next.id,
          artistName: next.artistName,
          artistNameZh: next.artistNameZh,
          city: next.city,
          cityZh: next.cityZh,
          date: next.date,
          lat: next.lat,
          lng: next.lng,
        }
      : null,
    byArtist,
    refreshedAt: new Date().toISOString(),
  };
}

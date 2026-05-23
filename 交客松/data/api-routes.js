/**
 *  Ticket API Routes — 供队友 A 前端调用的完整接口
 */
import { Router } from "express";
import {
  findNextConcert,
  getCountdown,
  getAllCountdowns,
  getPriceStatus,
  getAllPriceStatuses,
  simulatePurchase,
  getDashboardSummary,
} from "./ticket-engine.js";
import { readFileSync } from "fs";
import { join, dirname } from "path";
import { fileURLToPath } from "url";

const __dirname = dirname(fileURLToPath(import.meta.url));
const DATA_PATH = join(__dirname, "tour-data.json");

const router = Router();

// ── GET /api/ticket/dashboard — 首页仪表盘 ───────────────
router.get("/api/ticket/dashboard", (_req, res) => {
  res.json(getDashboardSummary());
});

// ── GET /api/ticket/countdown — 最近一场倒计时 ────────────
router.get("/api/ticket/countdown", (req, res) => {
  const { artist } = req.query;
  const result = getCountdown();
  if (artist) {
    const byArtist = getCountdown();
    if (result.concert?.artistId !== artist) {
      // Find next for specific artist
      const next = findNextConcert(artist);
      if (next) {
        return res.json(getCountdown(next.id));
      }
      return res.json({ error: "No upcoming concerts for this artist" });
    }
  }
  res.json(result);
});

// ── GET /api/ticket/countdown/all — 所有场次倒计时 ────────
router.get("/api/ticket/countdown/all", (req, res) => {
  const { artist } = req.query;
  res.json(getAllCountdowns(artist || null));
});

// ── GET /api/ticket/price/all — 全量价格（地图标记用）─────
router.get("/api/ticket/price/all", (req, res) => {
  const { artist } = req.query;
  res.json(getAllPriceStatuses(artist || null));
});

// ── GET /api/ticket/price/:concertId — 单场价格状态 ───────
router.get("/api/ticket/price/:concertId", (req, res) => {
  const result = getPriceStatus(req.params.concertId);
  if (result.error) return res.status(404).json(result);
  res.json(result);
});

// ── POST /api/ticket/purchase — 模拟购票（实时扣减）────────
router.post("/api/ticket/purchase", (req, res) => {
  const { concertId, quantity } = req.body;
  if (!concertId) return res.status(400).json({ error: "concertId required" });
  const result = simulatePurchase(concertId, quantity || 1);
  res.json(result);
});

// ── GET /api/ticket/data — 原始数据导出（静态 JSON dump）───
router.get("/api/ticket/data", (_req, res) => {
  const raw = readFileSync(DATA_PATH, "utf-8");
  res.json(JSON.parse(raw));
});

// ── GET /api/ticket/next — 下一场演唱会信息 ────────────────
router.get("/api/ticket/next", (req, res) => {
  const { artist } = req.query;
  const next = findNextConcert(artist || null);
  if (!next) return res.json({ error: "No upcoming concerts" });
  res.json(next);
});

export default router;

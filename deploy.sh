#!/bin/bash
# ===================================
#  虚拟宠物 · 一键联网部署脚本
#  朋友无需同 WiFi，0 注册即可玩
# ===================================

set -e

echo "🐾 启动虚拟宠物服务..."

# 1. 杀掉旧进程
npx kill-port 3456 2>/dev/null || true
sleep 1

# 2. 启动后端
cd "$(dirname "$0")"
node server.js &
SERVER_PID=$!
sleep 2

# 3. 验证后端
if ! curl -s http://localhost:3456/virtual-pet.html > /dev/null; then
  echo "❌ 后端启动失败"
  exit 1
fi
echo "✅ 后端启动成功 (PID: $SERVER_PID)"

# 4. 启动 serveo 公网隧道（零配置）
echo "🌐 正在获取公网地址..."
ssh -o StrictHostKeyChecking=no -o ServerAliveInterval=30 \
    -R 80:localhost:3456 serveo.net 2>&1 | while read line; do
  echo "$line"
  if echo "$line" | grep -q "Forwarding HTTP traffic from"; then
    URL=$(echo "$line" | grep -o 'https://[^ ]*')
    echo ""
    echo "========================================="
    echo "  🔗 公网地址（发给朋友）:"
    echo "  $URL/virtual-pet.html"
    echo "========================================="
    echo ""
  fi
done

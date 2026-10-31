//javascript真的诗人用的吗
const dt = 1 / 60;
const MAX_STEPS = 5;
const MAX_FRAME = MAX_STEPS * dt;

const g = 9.889;
const mDen = [1, 2, 3, 4];
const hp = [5, 15, 20, 25];

const screenH = 600;
const screenW = 800;
const ground = 50;
const scale = 20;
const BACKGROUND = '#ffffff';
const COLORS = ["#222222", "#555555", "#999999", "#cccccc"]
const width = 2.4;
const height = 0.4;
const sqaW = 0.8;
const structureP = [300 / scale, (screenH - ground) / scale];


const canvas = document.getElementById('myCanvas');
canvas.width = screenW
canvas.height  = screenH;
const ctx = canvas.getContext('2d');

//向量计算
const vAdd = (v1, v2) => [v1[0] + v2[0], v1[1] + v2[1]];
const vDot = (v1, v2) => v1[0] * v2[0] + v1[1] * v2[1];
const vSub = (v1, v2) => [v1[0] - v2[0], v1[1] - v2[1]];
const vProduceN = (angle) => [Math.cos(angle), Math.sin(angle)];
const vDis = (v1, v2) => ((v1[0] - v2[0]) ** 2 + (v1[1] - v2[1]) ** 2) ** (1 / 2)
const vCrossXY = (v1, v2) => v1[0] * v2[1] - v1[1] * v2[0];
const vCrossWl = (w, r) => [-w * r[1], w * r[0]];
const vInv = (v) => [-v[0], -v[1]];
const vLen = (v) => vDis(v, [0, 0]);

//type: 0木板 1石块 2
function createBody(o) {
  return {
    pos: [0, 0], angle: 0, v:[0, 0], omega: 0,
    invMass: 0, invI: 0, e: 0.5, u: 0.5,
    type: 1, w: 0, h: 0, r: 0, hp: 5,
    ...o,
  };
}

const bodies = [];

let score = 0;

//--------- 辅助函数 ----------
//----- 动画 -----
function clear() {
  ctx.fillStyle = BACKGROUND;
  ctx.fillRect(0, 0, screenW, screenH);
}

function draw_body(b) {
  ctx.save();
  ctx.translate(b.pos[0] * scale, b.pos[1] * scale);
  ctx.rotate(b.angle);
  ctx.fillStyle = COLORS[b.type];
  ctx.fillRect(-b.w * scale / 2, -b.h * scale / 2, b.w * scale, b.h * scale);
  ctx.restore();
}

function draw_ground() {
  ctx.fillStyle = "#000000";
  ctx.fillRect(0, screenH - ground, screenW, ground);
}

function draw_score() {
  ctx.fillStyle = "#000000";
  ctx.font = 'bold 30px Arial';
  ctx.fillText(String(score | 0), 10, 32);
}

const flip_coin = (p) => Math.random() > p;

function create_brick(typen, posi, anglep) {
  const invMass = 1 / (mDen[typen] * width * height);
  bodies.push(createBody({type: typen, pos: posi, w: width, h: height, angle: anglep,
                          invMass, invI: calInvI(invMass, width, height), hp: hp[typen]}));
}

function create_entity(typen, posi) {
  const invMass = 1 / (mDen[typen] * sqaW * sqaW);
  bodies.push(createBody({type: typen, pos: posi, w: sqaW, h: sqaW,
                          invMass, invI: calInvI(invMass, sqaW, sqaW), hp: hp[typen]}));
}

//地面做成静止刚体，和别的物体走同一套接触流形
function create_ground() {
  const w = screenW / scale * 4;
  const h = ground / scale * 8;

  bodies.push(createBody({type: 0, isGround: true, w, h, hp: Infinity, invMass: 0, invI: 0,
                          pos: [screenW / scale / 2, (screenH - ground) / scale + h / 2]}));
}

function random_produce_structure() {
  const structure = [
    [1, 1, 1, 1, 1, 1],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0],
  ]

  let hori = structureP[1];
  let p = 0.5;
  for(let y = 1; y < 5; y++) {
    let verti = structureP[0];
    for(let x = 1; x <= 5; x++) {
      if(structure[y - 1][x] && flip_coin(p)) {
        structure[y][x] = 1;
        p += 0.07;
        create_brick(flip_coin(0.5) ? 0 : 1, [verti + width / 2, hori - (width + height / 2)], 0);
        create_brick(flip_coin(0.5) ? 0 : 1, [verti + height / 2, hori - width / 2], Math.PI / 2);
        create_brick(flip_coin(0.5) ? 0 : 1, [verti + (width - height / 2), hori - width / 2], Math.PI / 2);
        create_entity(3, [verti + width / 2, hori - sqaW / 2]);
      }
      verti += width;
    }
    hori -= (width + height);
  }

}

function destructBody(b) {
  //TODO : 摧毁动画
  const index = bodies.indexOf(b)
  bodies.splice(index, 1);
}

function destructAll() {
  for(const b of bodies) {
    if (b.hp <= 0) {
      destructBody(b);
      score += 1 / b.invMass * 100 - b.hp * 1000;
    }
  }
}

//----- 物理 -----
const VEL_ITER = 32;

const calInvI = (invMass, w, h) => 12 / (w * w + h * h) * invMass

function PutJonBody(b, j, r) {
  b.v[0] += j[0] * b.invMass;
  b.v[1] += j[1] * b.invMass;
  b.omega += vCrossXY(r, j) * b.invI;
}

function PutJonBodyAndHP(b, j, r) {
  PutJonBody(b, j, r);
  if(vLen(j) > 0.5) b.hp -= (vLen(j) - 0.5);
}

//点是否在矩形内
function withinRect(pos, rPos, w, h, angle) {
  const rltv = vSub(pos, rPos);

  const rw = Math.abs(vDot(vProduceN(angle), rltv));
  const rh = Math.abs(vDot(vProduceN(angle + Math.PI / 2), rltv));

  return (rw < w / 2 + 1e-9) && (rh < h / 2 + 1e-9);
}

//按顺序返回矩形四个点
function calRectPoints(rPos, w, h, angle) {
  const u = [Math.cos(angle) * w / 2, Math.sin(angle) * w / 2];
  const v = [-Math.sin(angle) * h / 2, Math.cos(angle) * h / 2];

  return [
    [rPos[0] + u[0] + v[0], rPos[1] + u[1] + v[1]],
    [rPos[0] - u[0] + v[0], rPos[1] - u[1] + v[1]],
    [rPos[0] - u[0] - v[0], rPos[1] - u[1] - v[1]],
    [rPos[0] + u[0] - v[0], rPos[1] + u[1] - v[1]],
  ];
}

//按顺序返回矩形四条边的两端点和外法向
function calRectEdges(rPos, w, h, angle) {
  const p = calRectPoints(rPos, w, h, angle);
  const u = vProduceN(angle);
  const v = vProduceN(angle + Math.PI / 2);

  return [
    { a: p[0], b: p[1], n: v },
    { a: p[1], b: p[2], n: vInv(u) },
    { a: p[2], b: p[3], n: vInv(v) },
    { a: p[3], b: p[0], n: u },
  ];
}

//把线段p-q裁到 vDot(nrm, x) <= off 的一侧，返回剩下的点
function clipSeg(p, q, nrm, off) {
  const d1 = vDot(nrm, p) - off;
  const d2 = vDot(nrm, q) - off;

  if(d1 <= 0 && d2 <= 0) return [p, q];
  if(d1 > 0 && d2 > 0) return [];

  const s = d1 / (d1 - d2);
  const m = [p[0] + (q[0] - p[0]) * s, p[1] + (q[1] - p[1]) * s];

  return d1 <= 0 ? [p, m] : [m, q];
}

//找两矩形碰撞点，参考面裁剪入射面得到最多两个点
function findCollideP(b1, b2, n) {
  const vN1w = vProduceN(b1.angle);
  const vN1h = vProduceN(b1.angle + Math.PI / 2);
  const vN2w = vProduceN(b2.angle);
  const vN2h = vProduceN(b2.angle + Math.PI / 2);

  //法向贴近谁就把谁当参考盒，并列时固定取 b1
  const near1 = Math.max(Math.abs(vDot(n, vN1w)), Math.abs(vDot(n, vN1h)));
  const near2 = Math.max(Math.abs(vDot(n, vN2w)), Math.abs(vDot(n, vN2h)));
  const b1Ref = near1 >= near2;
  const R = b1Ref ? b1 : b2;
  const I = b1Ref ? b2 : b1;

  //参考面，取外法向最贴近"从参考盒指向对面"的那条边
  const refDir = b1Ref ? n : vInv(n);
  let refEdge = null, refBest = -Infinity;
  for(const e of calRectEdges(R.pos, R.w, R.h, R.angle)) {
    const d = vDot(e.n, refDir);
    if(d > refBest) { refBest = d; refEdge = e; }
  }

  //入射面，取外法向与参考面最反向的那条边
  let incEdge = null, incBest = Infinity;
  for(const e of calRectEdges(I.pos, I.w, I.h, I.angle)) {
    const d = vDot(e.n, refEdge.n);
    if(d < incBest) { incBest = d; incEdge = e; }
  }

  //把入射面裁到参考面两个端点之间
  const t = [-refEdge.n[1], refEdge.n[0]];
  const ta = vDot(t, refEdge.a), tb = vDot(t, refEdge.b);
  const tLo = Math.min(ta, tb), tHi = Math.max(ta, tb);

  let ps = clipSeg(incEdge.a, incEdge.b, vInv(t), -tLo);
  if(ps.length > 1) ps = clipSeg(ps[0], ps[1], t, tHi);

  //只留真正嵌进参考面的点
  const Ps = [];
  for(const p of ps) {
    if(vDot(refEdge.n, vSub(p, refEdge.a)) < 1e-9) Ps.push(p);
  }

  //一个都没剩就退回入射面上嵌得最深的端点
  if(!Ps.length) {
    const d1 = vDot(refEdge.n, vSub(incEdge.a, refEdge.a));
    const d2 = vDot(refEdge.n, vSub(incEdge.b, refEdge.a));
    Ps.push(d1 < d2 ? incEdge.a : incEdge.b);
  }

  return Ps;
}

//sat碰撞,碰撞则返回碰撞边法向量和法向穿透深度
function is_collided(b1, b2) {
  if(2 * vDis(b1.pos, b2.pos) > vDis([0, 0], [b1.w, b1.h]) + vDis([0, 0], [b2.w, b2.h])) return null;

  const vN1w = vProduceN(b1.angle);
  const vN1h = vProduceN(b1.angle + Math.PI / 2);
  const vN2w = vProduceN(b2.angle);
  const vN2h = vProduceN(b2.angle + Math.PI / 2);
  const vNs = [vN1w, vN1h, vN2w, vN2h];

  let minPen = Infinity;
  let nc = null;
  for (const n of vNs) {
    const proDis = Math.abs(vDot(vSub(b1.pos, b2.pos), n));
    const proW = (Math.abs(vDot(vN1w, n) * b1.w) + Math.abs(vDot(vN1h, n) * b1.h) + Math.abs(vDot(vN2w, n) * b2.w) + Math.abs(vDot(vN2h, n) * b2.h)) / 2;
    const pen = proW - proDis;
    if(pen < 0) return null;
    if(pen < minPen) { minPen = pen; nc = n; }
  }

  if(!nc) return null;
  if(vDot(vSub(b2.pos, b1.pos), nc) < 0) nc = vInv(nc);
  return [nc[0], nc[1], minPen];
}

//窄相，生成全部接触流形，每帧重建
function buildContacts() {
  const ms = [];

  for(let i = 0; i < bodies.length; i++) {
    for(let j = 0; j < bodies.length; j++) {
      if(i <= j) continue;
      const b1 = bodies[i];
      const b2 = bodies[j];
      const n = is_collided(b1, b2);
      if(!n) continue;

      const Ps = findCollideP(b1, b2, n);
      const m = { b1, b2, n, pts: [] };
      const t = [-n[1], n[0]];

      for(const P of Ps) {
        const r1 = vSub(P, b1.pos);
        const r2 = vSub(P, b2.pos);

        const vp1 = [b1.v[0] - b1.omega * r1[1], b1.v[1] + b1.omega * r1[0]];
        const vp2 = [b2.v[0] - b2.omega * r2[1], b2.v[1] + b2.omega * r2[0]];

        const rxN1 = vCrossXY(r1, n);
        const rxN2 = vCrossXY(r2, n);
        const rxT1 = vCrossXY(r1, t);
        const rxT2 = vCrossXY(r2, t);

        m.pts.push({
          r1, r2,
          vn0: vDot(vSub(vp2, vp1), n),
          k : b1.invMass + b2.invMass + b1.invI * rxN1 * rxN1 + b2.invI * rxN2 * rxN2,
          kf: b1.invMass + b2.invMass + b1.invI * rxT1 * rxT1 + b2.invI * rxT2 * rxT2,
          Pn: 0, Pt: 0,
        });
      }
      ms.push(m);
    }
  }

  return ms;
}

//顺序冲量，逐个流形逐个点求解，冲量在轮次之间累积
function solveContacts(ms) {
  for(const m of ms) {
    const b1 = m.b1, b2 = m.b2, n = m.n;
    const t = [-n[1], n[0]];
    const e = Math.min(b1.e, b2.e);
    const mu = (b1.u + b2.u) / 2;

    for(const p of m.pts) {
      if(p.k <= 0) continue;

      const vp1 = [b1.v[0] - b1.omega * p.r1[1], b1.v[1] + b1.omega * p.r1[0]];
      const vp2 = [b2.v[0] - b2.omega * p.r2[1], b2.v[1] + b2.omega * p.r2[0]];
      const v_n = vDot(vSub(vp2, vp1), n);

      const bias = p.vn0 < 0 ? e * p.vn0 : 0;
      const PnN  = Math.max(0, p.Pn - (v_n + bias) / p.k);
      const j    = PnN - p.Pn;
      p.Pn = PnN;

      PutJonBody(b1, [-j * n[0], -j * n[1]], p.r1);
      PutJonBody(b2, [j * n[0], j * n[1]], p.r2);

      if(p.kf <= 0) continue;

      const vp1b = [b1.v[0] - b1.omega * p.r1[1], b1.v[1] + b1.omega * p.r1[0]];
      const vp2b = [b2.v[0] - b2.omega * p.r2[1], b2.v[1] + b2.omega * p.r2[0]];
      const v_t = vDot(vSub(vp2b, vp1b), t);

      const PtN = Math.max(-mu * PnN, Math.min(mu * PnN, p.Pt - v_t / p.kf));
      const jf  = PtN - p.Pt;
      p.Pt = PtN;

      PutJonBody(b1, [-jf * t[0], -jf * t[1]], p.r1);
      PutJonBody(b2, [jf * t[0], jf * t[1]], p.r2);
    }
  }
}

//位置修正，按逆质量把穿透推开，迭代之外只做一次
function correctPositions(ms) {
  for(const m of ms) {
    const b1 = m.b1, b2 = m.b2, n = m.n;
    const invSum = b1.invMass + b2.invMass;
    if(invSum <= 0) continue;

    const corr = Math.max(n[2] - 0.0005, 0) / invSum;
    b1.pos[0] -= n[0] * corr * b1.invMass;
    b1.pos[1] -= n[1] * corr * b1.invMass;
    b2.pos[0] += n[0] * corr * b2.invMass;
    b2.pos[1] += n[1] * corr * b2.invMass;
  }
}

//结算伤害，用整个流形的法向冲量之和，迭代结束后只算一次
function applyHP(ms) {
  for(const m of ms) {
    let j = 0;
    for(const p of m.pts) j += p.Pn;

    if(j > 0.5) {
      m.b1.hp -= j - 0.5;
      m.b2.hp -= j - 0.5;
    }
  }
}

//处理碰撞
function collide() {

  //两物体间碰撞
  const ms = buildContacts();
  for(let iter = 0; iter < VEL_ITER; iter++) solveContacts(ms);
  correctPositions(ms);
  applyHP(ms);

}


//------------- 主循环 ----------------
function physics_process() {
  collide();
  destructAll();
  for(const b of bodies) {
    if(b.invMass === 0) continue;

    b.v[1] += g * dt;

    b.pos[0] += b.v[0] * dt;
    b.pos[1] += b.v[1] * dt;
    b.angle += b.omega * dt;
  }
}

let acc  = 0;
let last = performance.now() / 1000;

function get_input() {

}

create_ground();

random_produce_structure();

function frame(nowMs) {
  requestAnimationFrame(frame);

  const now = nowMs / 1000;
  let ft = now - last;
  last = now;
  if (ft > MAX_FRAME) ft = MAX_FRAME;

  acc += ft;

  let steps = 0;
  while (acc >= dt && steps < MAX_STEPS) {
    physics_process();
    acc -= dt;
    steps++;
  }
  if (steps === MAX_STEPS) acc = 0;

  clear();
  draw_ground();
  draw_score();
  for (const b of bodies) {
    if(b.isGround) continue;
    draw_body(b);
  }
}

requestAnimationFrame(frame);

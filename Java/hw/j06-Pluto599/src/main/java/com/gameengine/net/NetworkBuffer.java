package com.gameengine.net;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Deque;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public final class NetworkBuffer {
    private static final Deque<Keyframe> buffer = new ArrayDeque<>();
    private static final Object lock = new Object();
    private static final double MAX_AGE_SEC = 2.0;
    private static final double INTERP_DELAY_SEC = 0.12; // 120ms 缓冲

    public static class Entity {
        public String id; public float x; public float y;
    }
    public static class Keyframe {
        public double t;
        public List<Entity> entities = new ArrayList<>();
    }

    public static void push(Keyframe kf) {
        synchronized (lock) {
            buffer.addLast(kf);
            // 修剪老帧
            double now = kf.t;
            while (!buffer.isEmpty() && now - buffer.peekFirst().t > MAX_AGE_SEC) buffer.pollFirst();
        }
    }

    public static Keyframe parseJsonLine(String line) {
        // 极简 JSON 解析（假设格式固定）：{"type":"kf","t":X,"entities":[{"id":"...","x":N,"y":N},...]}
        if (line == null || !line.contains("\"type\":\"kf\"")) return null;
        Keyframe kf = new Keyframe();
        try {
            String ts = com.gameengine.recording.RecordingJson.field(line, "t");
            kf.t = com.gameengine.recording.RecordingJson.parseDouble(ts);
            int idx = line.indexOf("\"entities\":[");
            if (idx >= 0) {
                int bracket = line.indexOf('[', idx);
                String arr = bracket >= 0 ? com.gameengine.recording.RecordingJson.extractArray(line, bracket) : "";
                String[] parts = com.gameengine.recording.RecordingJson.splitTopLevel(arr);
                for (String p : parts) {
                    Entity e = new Entity();
                    e.id = com.gameengine.recording.RecordingJson.stripQuotes(com.gameengine.recording.RecordingJson.field(p, "id"));
                    e.x = (float)com.gameengine.recording.RecordingJson.parseDouble(com.gameengine.recording.RecordingJson.field(p, "x"));
                    e.y = (float)com.gameengine.recording.RecordingJson.parseDouble(com.gameengine.recording.RecordingJson.field(p, "y"));
                    kf.entities.add(e);
                }
            }
        } catch (Exception ignored) {}
        return kf;
    }

    public static Map<String, float[]> sample() {
        Keyframe prev;
        Keyframe next;
        double target = System.currentTimeMillis() / 1000.0 - INTERP_DELAY_SEC;
        synchronized (lock) {
            if (buffer.isEmpty()) {
                return Collections.emptyMap();
            }
            prev = buffer.peekFirst();
            next = buffer.peekLast();
            for (Keyframe kf : buffer) {
                if (kf.t <= target) {
                    prev = kf;
                }
                if (kf.t >= target) {
                    next = kf;
                    break;
                }
            }
        }

        if (prev == null) {
            return Collections.emptyMap();
        }
        if (next == null) {
            next = prev;
        }

        double span = Math.max(1e-6, next.t - prev.t);
        double u = Math.max(0.0, Math.min(1.0, (target - prev.t) / span));

        Map<String, Entity> prevMap = toEntityMap(prev);
        Map<String, Entity> nextMap = toEntityMap(next);
        Set<String> ids = new HashSet<>(prevMap.keySet());
        ids.addAll(nextMap.keySet());

        Map<String, float[]> out = new HashMap<>();
        for (String id : ids) {
            Entity ea = prevMap.get(id);
            Entity eb = nextMap.get(id);
            if (ea == null && eb == null) {
                continue;
            }
            if (ea == null) {
                ea = eb;
            }
            if (eb == null) {
                eb = ea;
            }
            if (ea == null) {
                continue;
            }
            float x = (float) ((1.0 - u) * ea.x + u * eb.x);
            float y = (float) ((1.0 - u) * ea.y + u * eb.y);
            out.put(id, new float[] { x, y });
        }
        return out;
    }

    private static Map<String, Entity> toEntityMap(Keyframe frame) {
        Map<String, Entity> map = new HashMap<>();
        if (frame == null || frame.entities == null) {
            return map;
        }
        for (Entity entity : frame.entities) {
            if (entity != null && entity.id != null) {
                map.put(entity.id, entity);
            }
        }
        return map;
    }
}



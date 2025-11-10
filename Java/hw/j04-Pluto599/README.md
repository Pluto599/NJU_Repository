[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/4IbaZuDC)

# j04 刘佳璇-231220105

游戏录屏：【【NJU】高级Java程序设计2025秋-j04】 https://www.bilibili.com/video/BV1GxkiB8EWx/?share_source=copy_web&vd_source=144cee55c0fac119fdd03eb502bb0b38

---

### 一、性能瓶颈分析

#### 1. **物理更新 (`updatePhysics()`)**
- **问题**: 随着游戏对象(敌人、子弹等)数量增加,需要遍历所有物理组件进行位置更新和边界检测
- **复杂度**: O(n),其中n是物理组件数量
- **瓶颈原因**: 当游戏中有大量对象时(例如数百个敌人和子弹),串行处理会导致每帧耗时显著增加
- **可并行化**: 每个物理组件的更新是独立的,不依赖其他组件的计算结果,适合并行处理

#### 2. **碰撞检测 (`checkCollisions()`)**
- **问题**: 需要检测所有游戏对象之间的碰撞
- **复杂度**: O(n²),其中n是游戏对象数量
- **瓶颈原因**: 碰撞检测需要对每个对象与其他所有对象进行距离计算,对象越多计算量越大
- **可并行化**: 可以将对象分组,每个线程处理一组对象的碰撞检测,最后合并结果

### 二、并行化实现方案

#### 1. **线程池设计**
```java
// 创建固定大小的线程池
int threadCount = Math.max(2, Runtime.getRuntime().availableProcessors() - 1);
ExecutorService physicsExecutor = Executors.newFixedThreadPool(threadCount);
```

#### 2. **并行物理更新实现**

**批次划分策略**:
```java
int batchSize = Math.max(1, physicsComponents.size() / threadCount + 1);
```

**关键代码**:
```java
for (int i = 0; i < physicsComponents.size(); i += batchSize) {
    final int start = i;
    final int end = Math.min(i + batchSize, physicsComponents.size());
    
    Future<?> future = physicsExecutor.submit(() -> {
        for (int j = start; j < end; j++) {
            PhysicsComponent physics = physicsComponents.get(j);
            updateSinglePhysics(physics);
        }
    });
    futures.add(future);
}

// 等待所有线程完成
for (Future<?> future : futures) {
    future.get();
}
```

#### 3. **并行碰撞检测实现**

**线程安全设计**:
- 使用 `ConcurrentHashMap` 存储碰撞结果,保证多线程写入安全
- 每个线程处理不同的对象批次,避免数据竞争

**关键代码**:
```java
// 预过滤:只处理有物理组件的对象
List<GameObject> physicsObjs = new ArrayList<>();
for (GameObject obj : objs) {
    if (obj.hasComponent(PhysicsComponent.class)) {
        physicsObjs.add(obj);
    }
}
```

#### 4. **性能监控**

添加了性能统计功能,每3秒输出一次平均性能数据:
```java
private long totalPhysicsTime = 0;
private long totalCollisionTime = 0;
private int frameCount = 0;

// 在每个方法中记录执行时间
long startTime = System.nanoTime();
// ... 执行逻辑 ...
long endTime = System.nanoTime();
totalPhysicsTime += (endTime - startTime);

// 定期输出统计信息
System.out.println(String.format(
                    "========== 性能统计 ========== | 物理更新: %.2fms | 碰撞检测: %.2fms | 总计: %.2fms | 帧数: %d",
                    avgPhysicsMs, avgCollisionMs, avgTotalMs, frameCount));
            }
```

### 性能对比测试

#### 测试环境
- 核心数: 16
- 游戏对象数量: 初始200个敌人,每0.1秒增加1个
- 为了测试方便，人物碰撞敌人不会死亡

#### 性能提升

**实测数据** (将在运行游戏后填写):
```
// 串行
物理更新: 0.16ms | 碰撞检测: 4.12ms | 总计: 4.28ms | 帧数: 156
物理更新: 0.05ms | 碰撞检测: 4.55ms | 总计: 4.61ms | 帧数: 180
物理更新: 0.05ms | 碰撞检测: 5.73ms | 总计: 5.77ms | 帧数: 179
物理更新: 0.05ms | 碰撞检测: 7.11ms | 总计: 7.15ms | 帧数: 180
物理更新: 0.04ms | 碰撞检测: 8.74ms | 总计: 8.78ms | 帧数: 180
物理更新: 0.05ms | 碰撞检测: 10.76ms | 总计: 10.81ms | 帧数: 181

// 并行
物理更新: 0.31ms | 碰撞检测: 0.66ms | 总计: 0.97ms | 帧数: 164
物理更新: 0.22ms | 碰撞检测: 0.33ms | 总计: 0.55ms | 帧数: 180
物理更新: 0.16ms | 碰撞检测: 0.34ms | 总计: 0.49ms | 帧数: 179
物理更新: 0.24ms | 碰撞检测: 0.43ms | 总计: 0.67ms | 帧数: 179
物理更新: 0.23ms | 碰撞检测: 0.43ms | 总计: 0.65ms | 帧数: 180
物理更新: 0.21ms | 碰撞检测: 0.50ms | 总计: 0.71ms | 帧数: 180
```

# Container Observability Platform

A lightweight observability platform built in C++ for monitoring Linux containers through direct interaction with Cgroups v2.

This project was developed as a companion system to my custom container runtime, where containers are created using Linux namespaces, chroot, and Cgroups. The platform automatically discovers running containers, collects resource metrics directly from kernel control files, and presents them through a live terminal dashboard.

## Motivation

After building a container runtime from scratch, I wanted visibility into how containers consumed system resources at runtime.

Rather than relying on external monitoring tools, I built a dedicated observability layer that directly reads Linux Cgroups v2 metrics and provides real-time insights into container resource utilization.

This mirrors the architecture used by modern observability systems, where lightweight agents collect kernel-level metrics and expose them to monitoring dashboards.

## Features

* **Automatic container discovery** through Cgroups v2 scanning
* **Real-time memory monitoring** and active sorting (descending by memory consumption)
* **Memory utilization % calculation** for limited containers
* **CPU utilization % monitoring** using stateful delta calculations
* **Process count monitoring** (`pids.current`)
* **Host Summary dashboard** (Total containers, memory, active PIDs) with a live 1s refresh
* **Modular architecture** separating discovery, metrics collection, and presentation layers

## System Architecture

```text
Custom Container Runtime
           │
           ▼
   Linux Cgroups v2
           │
           ▼
Container Discovery Layer (container.cpp)
           │
           ▼
 Metrics Collection Layer (metrics.cpp) — Stateful CPU delta tracking
           │
           ▼
 Terminal Dashboard (display.cpp) — Real-time memory sorting
```

## Project Structure

```text
collector/
├── main.cpp
├── container.cpp
├── container.h
├── metrics.cpp
├── metrics.h
├── display.cpp
└── display.h
```

### Discovery Layer

Responsible for identifying active containers by scanning Cgroup directories.

### Metrics Layer

Collects runtime statistics directly from Linux kernel control files, calculating CPU and memory metrics:

* memory.current
* memory.max (Memory % limit calculations)
* pids.current
* cpu.stat (Stateful CPU % delta calculations)

### Presentation Layer

Formats and displays collected metrics in a continuously refreshing terminal dashboard.

## Technologies

* C++
* Linux
* Cgroups v2
* Filesystem API (C++17)
* Systems Programming

## Example Output

```text
COP Monitor

Containers: 2
Total Memory: 1.48 MB
Total PIDs:   2
Refresh:      1s

CONTAINER           MEM(MB)     LIMIT          MEM%      PIDS    CPU%    
---------------------------------------------------------------------------
mycontainer-5513    1.21        100 MB         1.2%      1       0.12%
mycontainer-5539    0.27        Unlimited      -         1       0.05%
```

## Future Improvements

* Historical metric storage and logging
* REST API layer for metrics exporting
* Interactive curses/ncurses dashboard
* Alerting and threshold-based monitoring

## Key Learning Outcomes

* Linux container internals
* Cgroups v2 resource management
* Systems programming in C++ (CPU & Memory delta tracking)
* Observability architecture
* Runtime metric collection
* Modular software design

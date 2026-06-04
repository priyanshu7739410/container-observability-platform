# Container Observability Platform

A lightweight observability platform built in C++ for monitoring Linux containers through direct interaction with Cgroups v2.

This project was developed as a companion system to my custom container runtime, where containers are created using Linux namespaces, chroot, and Cgroups. The platform automatically discovers running containers, collects resource metrics directly from kernel control files, and presents them through a live terminal dashboard.

## Motivation

After building a container runtime from scratch, I wanted visibility into how containers consumed system resources at runtime.

Rather than relying on external monitoring tools, I built a dedicated observability layer that directly reads Linux Cgroups v2 metrics and provides real-time insights into container resource utilization.

This mirrors the architecture used by modern observability systems, where lightweight agents collect kernel-level metrics and expose them to monitoring dashboards.

## Features

* Automatic container discovery through Cgroups v2
* Real-time memory monitoring
* Memory limit tracking
* Process count monitoring
* CPU usage collection
* Live terminal dashboard with periodic refresh
* Modular architecture separating discovery, metrics collection, and presentation layers

## System Architecture

```text
Custom Container Runtime
           │
           ▼
   Linux Cgroups v2
           │
           ▼
Container Discovery Layer
           │
           ▼
 Metrics Collection Layer
           │
           ▼
 Terminal Dashboard
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

Collects runtime statistics directly from Linux kernel control files:

* memory.current
* memory.max
* pids.current
* cpu.stat

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
CONTAINER           MEM(MB)     LIMIT        PIDS     CPU(usec)

mycontainer-5539    0.27        Unlimited    1        968
mycontainer-5513    1.21        Unlimited    1        2034
```

## Future Improvements

* CPU utilization percentages
* Memory utilization percentages
* Container ranking by resource consumption
* Historical metric storage
* REST API layer
* Web dashboard
* Alerting and threshold-based monitoring

## Key Learning Outcomes

* Linux container internals
* Cgroups v2 resource management
* Systems programming in C++
* Observability architecture
* Runtime metric collection
* Modular software design

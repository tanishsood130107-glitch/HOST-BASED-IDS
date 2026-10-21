# HIDS: Intelligent Host Security Monitoring System

A modular, rule-based **Host-Based Intrusion Detection System (HIDS)** developed in C++ for Windows. The project monitors selected host activities, processes security-related events, and identifies potentially suspicious behavior using configurable detection rules.

## Overview

The HIDS prototype is designed to monitor important system activities and generate alerts when predefined security rules are matched. It follows a modular, object-oriented approach in which monitoring components collect host activity, convert it into common event representations, and pass it through an event-processing and rule-evaluation pipeline.

The current implementation focuses on failed login attempts, newly observed processes, and file changes within a configured directory.

## Objectives

- Monitor selected Windows host activities for potentially suspicious behavior.
- Detect failed login attempts, newly observed processes, and file changes.
- Represent different types of host activity using a common event structure.
- Filter relevant events before detection and analysis.
- Evaluate events against configurable rules, thresholds, keywords, severity levels, and time windows.
- Generate console-based security alerts when detection conditions are satisfied.
- Maintain a modular architecture that supports future improvements and additional monitoring capabilities.

## Key Features

- **Login Monitoring:** Collects information associated with failed logon events from the Windows Security Event Log.
- **Process Monitoring:** Identifies newly observed processes using Windows process information.
- **File Monitoring:** Tracks file creation, modification, and deletion within a configured directory.
- **Event Collection:** Organizes detected activities into a common event-processing workflow.
- **Event Filtering:** Filters events so relevant activities can be evaluated by the detection system.
- **Rule-Based Detection:** Evaluates events using configurable conditions, thresholds, keywords, severity levels, and time windows.
- **Security Alerts:** Displays console alerts when configured detection rules match observed events.
- **Modular Design:** Separates monitoring, event management, detection, and alert handling into distinct components.

## Project Modules

| Module | Responsibility |
|---|---|
| `main.cpp` | Entry point of the application and coordination of the monitoring workflow. |
| `LoginMonitor` | Collects information related to failed Windows logon events. |
| `ProcessMonitor` | Observes processes and identifies newly observed process activity. |
| `FileMonitor` | Detects file creation, modification, and deletion in the monitored directory. |
| `Event` | Defines the common representation of detected host activities. |
| `EventCollector` | Collects events for subsequent processing. |
| `EventFilter` | Filters events before rule evaluation. |
| `Rule` | Defines the detection conditions and configuration for security rules. |
| `RuleEngine` | Evaluates events against the configured detection rules. |
| `Alert` | Represents and supports the presentation of security alerts. |
| `HIDS` | Contains an additional event-processing workflow. |

The codebase also contains supporting components and headers for event collection, monitoring, detection, and alert handling.

## Technology Stack

- **Programming Language:** C++
- **Platform:** Microsoft Windows
- **Programming Approach:** Object-Oriented Programming (OOP)
- **Standard Libraries:** C++ Standard Library and filesystem library
- **Windows APIs:** Windows Event Log utilities and the Windows Tool Help API
- **Development Environment:** Visual Studio Code or another compatible C++ development environment

## Workflow

1. **Monitor:** The monitoring modules observe selected host activities.
2. **Create Events:** Detected activities are represented using the common `Event` structure.
3. **Collect:** The event collector gathers events for processing.
4. **Filter:** The event filter selects relevant events.
5. **Evaluate:** The rule engine checks events against configured detection conditions.
6. **Generate Alerts:** When a rule matches, the system produces a console-based security alert.
7. **Validate:** The monitoring and detection components are checked through compilation, functional testing, and integration testing.

## Testing and Validation

The Phase-II project report records the following status:

- Application compilation and execution: Working
- Login monitoring: Working
- Process monitoring: Working
- File monitoring: Working
- Event filtering: Working
- Rule engine: Working
- Console alert generation: Working

Further integration testing, rule-engine improvements, and validation of the complete system remain part of the ongoing development work. These statuses reflect the project report and should be updated as additional testing is completed.

## Current Scope and Future Improvements

The project provides a foundation for host-level security monitoring and rule-based detection. Planned improvements include:

- Improving rule evaluation and detection reliability.
- Refining detection thresholds and time-window conditions.
- Strengthening event handling, error handling, and system integration.
- Improving alert monitoring and management.
- Integrating virus signatures for malware detection.
- Developing the user interface.
- Expanding testing, validation, and technical documentation.

## Team

**Team Name:** Neural Geeks

- **Tanish Sood** — Team Lead
- **Jitender Singh** — Team Member
- **Chirag Varshney** — Team Member
- **Shambhavi Srivastava** — Team Member

## Project Status

**Phase:** Phase II — Project Progress

The project has progressed from system design to the implementation of a modular C++ prototype. Core monitoring, event-processing, rule-evaluation, and console-alert components have been developed, with further refinement and system-level validation planned.

---

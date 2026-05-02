# Thread-in-Mobile Demo

The sleepy-demo-wc (WC) and sleepy-demo-wed (WED) applications demonstrate the Wake-up Coordinator (WC) and Wake-up End Device (WED) functionalities using enhanced Coordinated Sample Listening (eCSL). Both applications are configured with RxOffWhenIdle.

## 1. Overview

- **sleepy-demo-wc node**: Acts as the Wake-up Coordinator. The WC is a role that a Thread Device can play to wake (activate) a Wake-up End Device (WED) and request the WED to connect to a Parent. To do this, the WC will send a dedicated wake-up frame to the WED on a wake-up channel to which the WED is listening periodically.

- **sleepy-demo-wed node**: Acts as the Wake-up End Device which is a Thread End Device. WED will periodically wake up to listen for wake-up messages on a dedicated wake-up radio channel. These wake-up frames can be sent by a Wake-up Coordinator (WC). Once a WED receives a valid, secured wake-up frame it will attempt to connect, as a Child, to the Wake-up Parent.

## 2. Starting the Applications

For demonstration purposes the applications are pre-configured with Thread network dataset within the source files. In a real-life application the devices should implement and go through a commissioning process to create a network and add devices.

When **_sleepy-demo-wc_** boots up, device will form a thread network within few seconds. This can be checked using OpenThread CLI command

```console
> state
leader
done
```

When **_sleepy-demo-wed_** boots up, device will start sampling for the wake-up frame with sampling rate of 8 msec per second. The terminal log should show `Listening for wakeup frames` message or issue CLI command to get the wake-up link state.

```console
> wakeup state
enabled
done
```

## 3. Device Linking

To establish a link between the WC and WED, the WC transmits wake-up frames on the wake-up channel (default: channel 11). Once the WED receives a wake-up frame, it starts the linking process. The devices remain connected until the session ends or synchronization is lost.

The WC should transmit the wake-up frames at a 7.5 msec interval for a duration of 1000 msec.
Issue this CLI command on **_sleepy-demo-wc_** terminal to wake-up the WED

```console
> wakeup wake 12ab34cd56ef78ab 7500 1000
Done
```

Wake-up end device samples for 8 msec in 1 sec interval to receive a potential wake-up frame. To check the wake-up parameters on **_sleepy-demo-wed_** terminal, issue

```console
> wakeup parameters
interval: 1000000us
duration: 8000us
Done
```

Issue this CLI command on **_sleepy-demo-wed_** terminal to check the state.

```console
> wakeup state
linked
Done
```

These are the different wake-up states and their representation.
| State | Representation |
| --------- | ---------------------------------------- |
| Disabled | Not sampling for wake-up frame |
| Enabled | Sampling for wake-up frame |
| Linking | In the process of the linking with WC |
| Linked | Linked with WC and ready to communicate |

### Supported Wake-up CLI commands

Visit OpenThread CLI document to get the list of **_wakeup_** CLIs.

## 4. Button on the WED

Button 0 toggles between EM2 (sleep) and EM1 (idle) modes.

## 5. Power Consumption Monitoring

Open the Energy Profiler in Simplicity Studio 5 (SSv5). In the Quick Access menu select **Start Energy Capture...** and select the WED device.

- In EM2 (sleep) mode, WED current should be under 10 µA, with spikes during wake-up and CSL windows.
- In EM1 (idle) mode, current is typically in the oder of few mA.
- Further GPIO and peripheral configuration can reduce sleep current.

## 6. Sleep Callback and Interrupts

To enable low power operation, register a sleep callback with `efr32AllowSleepCallback`. The callback should return a value indicating if the application can safely enter sleep mode. It is called with interrupts disabled, so keep logic minimal.

**_NOTE:_** Do not enable verbose logging because it may interfere with closely timed scheduling of transmissions and receptions.

# cuddy-init

*cuddy-init* is a light-weight "keep-container-alive" binary. Besides that, you can use *cuddy-init* to configure your container, either at startup or as a recurring task. You can also get feedback if the recurring task fails.

As the options might reveal it's a fellow companion with [*cuddy-client*](https://github.com/micusto/cuddy-client) (it's default, but not required).

### Usage:

`cuddy-init [OPTION]`

### Options:

| option | type   | default        | description                                                     |
|--------|--------|----------------|-----------------------------------------------------------------|
| -c     | string | `cuddy-client` | A command to execute every cycle (use `-c` to disable).         |
| -d     | int    | `240`          | The delay between cycles.                                       |
| -e     | string | *empty*        | A command to execute if `-c` exit code > 0.                     |
| -h     | bool   | `false`        | Display help.                                                   |
| -i     | string | *empty*        | An initial command to execute before the cycle starts.          |
| -s     | int    | `60`           | Used to generate a random number to delay between each cycle.   |
| -v     | bool   | `false`        | Verbose output.                                                 |

#### Usage examples:

The simplest form of usage is:

`cuddy-init`

This will execute the binary `cuddy-client` every 4-5 min. in case `cuddy-client` has an exit code > 0 the container will exit. Here is a slightly more *advanced* use case:

`cuddy-init -i "/initial.sh" -e "/error.sh"`

This will execute `/initial.sh` when the container starts, then execute `cuddy-client` every 4-5 min. and `cuddy-client` returns an exit code > 0, execute `/error.sh`.

Or you could use your own `/recurring.sh` script instead of `cuddy-client` using the `-c` option.

**Note:** *If `/error.sh` returns exit code: `0` cuddy-init will continue the cycle, otherwise it will exit with the exit code of the failing `-c`, meaning that this is a way of trying to handle an error inside your container, without having to restart.*

### Dockerfile:

This is a *quickstart* Dockerfile, modify it to your needs.

```
FROM debian:latest

MAINTAINER Henrik Urlund <henrik@cuddy.io>

# install top/ps command
RUN apt-get update \
    && apt-get install -y procps

# https://github.com/krallin/tini/blob/master/README.md
ADD https://github.com/krallin/tini/releases/download/v0.16.1/tini /usr/local/sbin/tini

# https://github.com/micusto/cuddy-init/blob/master/README.md
ADD https://github.com/micusto/cuddy-init/releases/download/v1.0.0/cuddy-init-linux-amd64 /usr/local/sbin/cuddy-init

# https://github.com/micusto/cuddy-client/blob/master/README.md
ADD https://github.com/micusto/cuddy-client/releases/download/v1.0.0/cuddy-client-linux-amd64 /usr/local/sbin/cuddy-client

# ensure binaries are executable - and make cuddy dir
RUN chmod +x /usr/local/sbin/tini \
    && chmod +x /usr/local/sbin/cuddy-init \
    && chmod +x /usr/local/sbin/cuddy-client \
    && mkdir -p /etc/cuddy/ \
    && echo "{}" > /etc/cuddy/config.json \
    && echo "[]" > /etc/cuddy/client.json

# https://github.com/krallin/tini/blob/master/README.md
ENTRYPOINT ["tini", "--" ]

# keep the container alive
CMD [ "cuddy-init" ]
```

In case you are wondering about the memory footprint:

```
PID USER      PR  NI    VIRT    RES    SHR S  %CPU %MEM     TIME+ COMMAND
  1 root      20   0    4.1m   0.7m   0.6m S   0.0  0.0   0:00.03 tini
  7 root      20   0    4.0m   0.7m   0.6m S   0.0  0.0   0:00.00 cuddy-init
```

## Important notice

`cuddy-client` is not yet public available, but in case you are interested, drop me a line.

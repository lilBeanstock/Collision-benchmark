FROM alpine:3 AS base

# Raylib dependencies.
# RUN apk add --no-cache gcc build-essential git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
RUN apk add --no-cache gcc build-base git alsa-lib-dev libx11-dev libxrandr-dev libxi-dev mesa-dev glu-dev libxcursor-dev libxinerama-dev wayland-dev libxkbcommon-dev

FROM base AS installer

RUN git clone --depth 1 https://github.com/raysan5/raylib.git raylib
WORKDIR /raylib/src
# To make the static version.
RUN make PLATFORM=PLATFORM_DESKTOP
RUN make install

FROM base AS runner

COPY --from=installer /usr/local /usr/local

# https://code.visualstudio.com/remote/advancedcontainers/sharing-git-credentials#_sharing-gpg-keys
# Coding environment configuration.
RUN apk add --no-cache bash bash-completion gnupg clang-dev

RUN addgroup -S devuser && adduser -S -G devuser devuser && mkdir -p /home/devuser && chown -R devuser:devuser /home/devuser
RUN echo "PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@\h\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '" >> /home/devuser/.bashrc
USER devuser

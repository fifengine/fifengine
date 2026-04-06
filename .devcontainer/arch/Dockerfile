FROM archlinux:latest

RUN echo "[PACMAN] Install packages" && \
    pacman -Syu --noconfirm \
    base-devel \
    git \
    sudo \
    zsh \
    curl wget \
    unzip tar xz \
    openssl ca-certificates \
    # Build tools
    extra/clang \
    extra/cmake \
    extra/ninja && \
    echo "[PACMAN] Clean up" && \
    rm -rf /var/cache/pacman/pkg/*

RUN echo "[ZSH] Configure zsh as default shell for root" && \
    echo "/usr/bin/zsh" >> /etc/shells && \
    chsh -s /usr/bin/zsh root && \
    echo "[ZSH] Install Oh My Zsh" && \
    sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended && \
    echo "[ZSH] Set Theme" && \
    sed -i 's/ZSH_THEME=.*/ZSH_THEME="robbyrussell"/' /root/.zshrc && \
    echo "[ZSH] Install plugins" && \
    git clone https://github.com/zsh-users/zsh-autosuggestions /usr/share/zsh/plugins/zsh-autosuggestions && \
    git clone https://github.com/zsh-users/zsh-syntax-highlighting /usr/share/zsh/plugins/zsh-syntax-highlighting && \
    echo "source /usr/share/zsh/plugins/zsh-autosuggestions/zsh-autosuggestions.zsh" >> /root/.zshrc && \
    echo "source /usr/share/zsh/plugins/zsh-syntax-highlighting/zsh-syntax-highlighting.zsh" >> /root/.zshrc

RUN echo "[Status]" && \
    clang --version && \
    cmake --version

RUN echo "[FIFEGUI] Dependencies" &&
    pacman -Su --noconfirm \
    # For building FifeGUI we need the following dependencies
    extra/sdl2 \
    extra/sdl2_image \
    extra/sdl2_mixer \
    extra/sdl2_ttf \
    extra/freetype2 \
    extra/utf8cpp \
    extra/catch2

RUN cd /tmp && \
    echo "[FIFEGUI] Get Repo" && \
    git clone https://github.com/fifengine/fifechan.git && \
    cd fifechan && \
    git checkout main && \
    mkdir build && \
    cd build && \
    echo "[FIFEGUI] Configure" && \
    cmake .. \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=ON \
      -DUSE_VCPKG=OFF \
      -DCMAKE_INSTALL_PREFIX=/out/fife-dependencies/x64-linux/install && \
    echo "[FIFEGUI] Build" && \
    make -j$(nproc) && \
    echo "[FIFEGUI] Install" && \
    make install && \
    echo "[FIFEGUI] Verify" && \
    ls /out/fife-dependencies/x64-linux/install/lib/libfifechan* && \
    ls /out/fife-dependencies/x64-linux/install/lib/cmake/fifechan/*

RUN echo "[FIFE] Dependencies" && \
    pacman -Su --noconfirm \
    # For building the Python bindings we need SWIG
    extra/swig \
    # For building Fife we need the following dependencies
    extra/openal \
    extra/tinyxml2 \
    extra/libvorbis \
    extra/libogg

RUN cd /workspaces/fifengine && \
    mkdir -p out/build && \
    cd out/build && \
    echo "[FIFE] Configure" && \
    cmake ../.. \
      -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_SHARED_LIBS=ON \
      -DBUILD_LIBRARY=ON \
      -DBUILD_PYTHON_BINDINGS=ON \
      -DENABLE_FIFEGUI=ON \
      -DUSE_VCPKG=OFF \
      -DCMAKE_PREFIX_PATH=/out/fife-dependencies/x64-linux/install
    echo "[FIFE] Build" && \
    make -j$(nproc) && \
    echo "[FIFE] Install" && \
    make install

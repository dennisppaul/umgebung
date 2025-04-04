#!/bin/bash

# TODO this is not tested on linux

check_brew() {
    if command -v brew >/dev/null 2>&1; then
        echo "Homebrew is already installed."
    else
        echo "Homebrew is not installed. Installing Homebrew..."
        install_brew
    fi
}

install_brew() {
    sudo apt-get update
    sudo apt-get install -y build-essential procps curl file git

    # install Homebrew
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    
    if [ $? -eq 0 ]; then
        echo "Homebrew installation successful."
    else
        echo "Homebrew installation failed."
        exit 1
    fi

    # add Homebrew to the PATH
    echo 'eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)"' >> ~/.profile
    eval "$(/home/linuxbrew/.linuxbrew/bin/brew shellenv)"
}

echo "+++ setting up umgebung   +++"

echo "+++ checking for homebrew +++"
check_brew

echo "+++ installing packages   +++"
brew bundle

echo
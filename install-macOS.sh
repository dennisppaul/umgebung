#!/bin/bash

check_brew() {
    if command -v brew >/dev/null 2>&1; then
        echo "Homebrew is already installed."
    else
        echo "Homebrew is not installed. Installing Homebrew..."
        install_brew
    fi
}

install_brew() {
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    if [ $? -eq 0 ]; then
        echo "Homebrew installation successful."
    else
        echo "Homebrew installation failed."
        exit 1
    fi
}

echo "+++ setting up umgebung   +++"

echo "+++ checking for homebrew +++"
check_brew

echo "+++ installing packages   +++"
brew bundle

echo
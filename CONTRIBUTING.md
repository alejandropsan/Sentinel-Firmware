# Contributing to Sentinel Firmware

Thank you for your interest in contributing to Sentinel Firmware! We welcome contributions from the community to help make this firmware even better.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Getting Started](#getting-started)
- [Development Environment](#development-environment)
- [Contribution Workflow](#contribution-workflow)
- [Coding Standards](#coding-standards)
- [Testing](#testing)
- [Pull Request Guidelines](#pull-request-guidelines)
- [Community](#community)

---

## Code of Conduct

By participating in this project, you agree to maintain a respectful and collaborative environment. We expect all contributors to:

- Be respectful and inclusive
- Provide constructive feedback
- Focus on what is best for the community
- Show empathy towards other community members

## Getting Started

### Prerequisites

Before contributing, make sure you have:

- A Flipper Zero device
- Basic knowledge of C programming
- Git installed on your system
- Linux, macOS, or WSL2 (Windows Subsystem for Linux)

### Development Tools

- **Python 3.11+** for build tools
- **Git** for version control
- **FBT** (Flipper Build Tool) - included in the repository
- Text editor or IDE with C language support

## Development Environment

### 1. Fork and Clone

```bash
# Fork the repository on GitHub, then clone your fork
git clone --recursive https://github.com/YOUR_USERNAME/Sentinel-Firmware.git
cd Sentinel-Firmware
```

### 2. Set Up Upstream Remote

```bash
git remote add upstream https://github.com/alejandropsan/Sentinel-Firmware.git
git fetch upstream
```

### 3. Install Dependencies

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y git wget libusb-1.0-0-dev

# macOS
brew install git wget libusb
```

### 4. Build the Firmware

```bash
# Build full firmware package
./fbt updater_package

# Or flash directly to device via USB
./fbt flash_usb_full
```

## Contribution Workflow

### 1. Create a Feature Branch

Always create a new branch for your work:

```bash
git checkout dev
git pull upstream dev
git checkout -b feature/your-feature-name
```

Branch naming conventions:
- `feature/description` - New features
- `fix/description` - Bug fixes
- `docs/description` - Documentation updates
- `refactor/description` - Code refactoring
- `security/description` - Security improvements

### 2. Make Your Changes

- Follow the [Coding Standards](#coding-standards) (see CODING_STYLE.md)
- Write clean, readable code
- Add comments where necessary
- Keep commits focused and atomic

### 3. Test Your Changes

```bash
# Run linter
./fbt lint

# Check code formatting
./fbt format_check

# Build and test
./fbt flash_usb_full
```

### 4. Commit Your Changes

Write clear, descriptive commit messages:

```bash
git add .
git commit -m "feat: Add duress PIN authentication system

- Implement PIN verification logic
- Add secure wipe functionality
- Include unit tests for PIN validation"
```

Commit message format:
```
<type>: <subject>

<body>

<footer>
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation changes
- `style`: Code style changes (formatting, etc.)
- `refactor`: Code refactoring
- `test`: Adding or updating tests
- `chore`: Maintenance tasks
- `security`: Security improvements

### 5. Push and Create Pull Request

```bash
git push origin feature/your-feature-name
```

Then create a Pull Request on GitHub targeting the `dev` branch.

## Coding Standards

Please read [CODING_STYLE.md](CODING_STYLE.md) for detailed coding standards. Key points:

- **Indentation**: 4 spaces (no tabs)
- **Line Length**: 99 characters maximum
- **Naming**: `snake_case` for functions and variables
- **Braces**: K&R style (opening brace on same line)
- **Comments**: Doxygen-style for public APIs
- **Format**: Use `./fbt format` before committing

## Testing

### Running Tests

```bash
# Run unit tests
./fbt firmware_test

# Run on-device tests
./fbt debug
```

### Writing Tests

- Add unit tests for new functionality
- Test edge cases and error conditions
- Ensure tests pass before submitting PR

### Manual Testing

- Test on actual hardware when possible
- Verify functionality across different scenarios
- Check for memory leaks and performance issues

## Pull Request Guidelines

### Before Submitting

- [ ] Code builds without errors
- [ ] Code passes linter (`./fbt lint`)
- [ ] Code is properly formatted (`./fbt format`)
- [ ] Tests are added/updated and passing
- [ ] Documentation is updated if needed
- [ ] CHANGELOG.md is updated (for significant changes)
- [ ] Commit messages follow conventions
- [ ] Branch is up-to-date with `dev`

### PR Description Template

```markdown
## Description
Brief description of what this PR does

## Type of Change
- [ ] Bug fix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation update

## Testing
How has this been tested?

## Checklist
- [ ] My code follows the style guidelines
- [ ] I have performed a self-review
- [ ] I have commented my code where needed
- [ ] I have updated the documentation
- [ ] My changes generate no new warnings
- [ ] I have added tests that prove my fix is effective or that my feature works
- [ ] New and existing unit tests pass locally

## Screenshots (if applicable)
Add screenshots to help explain your changes

## Related Issues
Closes #(issue number)
```

### Review Process

1. **Automated Checks**: CI/CD will run build, lint, and format checks
2. **Code Review**: Maintainers will review your code
3. **Feedback**: Address any requested changes
4. **Approval**: Once approved, your PR will be merged
5. **Release**: Changes will be included in the next release

### What to Expect

- Initial response within 3-5 days
- Constructive feedback on code quality
- Possible requests for changes or improvements
- Merger after approval from maintainers

## What We're Looking For

### High Priority

- **Security Features**: Duress PIN, encrypted storage, audit logging
- **Scripting**: DuckyScript 3.0, MicroPython support
- **Signal Analysis**: Spectrum analyzer, protocol decoder
- **Bug Fixes**: Any reproducible bugs
- **Documentation**: Improvements to guides and API docs
- **Performance**: Memory optimization, battery life improvements

### Also Welcome

- UI/UX improvements
- Additional applications
- Hardware integration
- Testing infrastructure
- Translation and localization

### Not Accepting

- Malicious or harmful code
- Copyright violations
- Breaking changes without discussion
- Unrelated features
- Code without proper attribution

## Reporting Bugs

### Before Reporting

1. Check existing issues
2. Update to latest firmware
3. Try to reproduce the bug
4. Gather relevant information

### Bug Report Template

```markdown
**Describe the bug**
A clear description of what the bug is

**To Reproduce**
Steps to reproduce:
1. Go to '...'
2. Click on '...'
3. See error

**Expected behavior**
What you expected to happen

**Actual behavior**
What actually happened

**Environment**
- Firmware version: [e.g., v0.1.0]
- Hardware version: [e.g., Flipper Zero f7]
- SD card: [Yes/No]
- Additional modules: [WiFi dev board, etc.]

**Logs**
Paste any relevant logs or error messages

**Screenshots**
If applicable, add screenshots
```

## Requesting Features

We welcome feature requests! Please:

1. Check if the feature is already in the [ROADMAP.md](ROADMAP.md)
2. Search existing feature requests
3. Open a [GitHub Discussion](https://github.com/alejandropsan/Sentinel-Firmware/discussions) first
4. If approved, create a detailed feature request issue

### Feature Request Template

```markdown
**Feature Description**
Clear description of the feature

**Use Case**
Why is this feature needed?

**Proposed Solution**
How should this feature work?

**Alternatives Considered**
What alternatives have you considered?

**Additional Context**
Any other information or screenshots
```

## Community

### Getting Help

- **Documentation**: Read the [Wiki](https://github.com/alejandropsan/Sentinel-Firmware/wiki)
- **Discussions**: Join [GitHub Discussions](https://github.com/alejandropsan/Sentinel-Firmware/discussions)
- **Discord**: Coming soon
- **Issues**: Search existing issues first

### Communication Channels

- **GitHub Issues**: For bugs and feature requests
- **GitHub Discussions**: For questions and general discussion
- **Pull Requests**: For code contributions

## License

By contributing to Sentinel Firmware, you agree that your contributions will be licensed under the [GPL-3.0 License](LICENSE).

All contributed code must be original work or properly attributed. Do not submit copyrighted code without permission.

## Acknowledgments

Sentinel Firmware is built on [Momentum Firmware](https://github.com/Next-Flip/Momentum-Firmware). We thank all contributors who have built this foundation.

---

Thank you for contributing to Sentinel Firmware! Together we can build a secure, powerful, and community-driven firmware for the Flipper Zero.

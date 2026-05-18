#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>

int main() {
    using namespace ftxui;

    // Create a simple UI layout
    Element document = vbox({
        text("--- TASKFLOW INTRO ---") | bold | color(Color::Blue),
        separator(),
        text("FTXUI is successfully configured!"),
        text("Ready to build Day 1 custom data structures."),
    }) | border;

    auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(document));
    Render(screen, document);
    screen.Print();

    return 0;
}
#include <iostream>

using namespace std;

void print_progress_bar(int completed, int total, int bar_width = 50) {
    // Calculate progress
    completed += 1;
    float progress = static_cast<float>(completed) / total; 
    int pos = static_cast<int>(bar_width * progress); 

    if (completed == total) cout << "\u001b[32m";

    // Print the progress bar
    cout << completed << "/" << total << " [";
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos)
            cout << "█"; // Completed part
        else
            cout << " "; // Incomplete part
    }
    cout << "] " << static_cast<int>(progress * 100) << "%"; // Update percentage

    // Move the cursor back to the start of the line for the next update
    if (completed < total) {
        cout << "\r"; // Carriage return to overwrite the line
    } else {
        cout << "\u001b[0m\n\n"; // New line after completion
    }
    cout.flush(); // Ensure the output is displayed immediately
}
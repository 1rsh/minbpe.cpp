#ifndef PROGRESSBAR_HPP   // Start of include guard
#define PROGRESSBAR_HPP

#include <iostream>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// Function to convert seconds into hours, minutes, and seconds
string format_time(double seconds) {
    int h = static_cast<int>(seconds) / 3600;
    int m = (static_cast<int>(seconds) % 3600) / 60;
    int s = static_cast<int>(seconds) % 60;
    
    stringstream ss;
    ss << setfill('0') << setw(2) << h << ":" << setw(2) << m << ":" << setw(2) << s;
    return ss.str();
}

void print_progress_bar(int completed, int total, steady_clock::time_point start_time, int bar_width = 50) {
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
    cout << "] " << static_cast<int>(progress * 100) << "% "; // Update percentage

    // Calculate elapsed time and ETA
    auto now = steady_clock::now();
    auto elapsed_seconds = duration_cast<seconds>(now - start_time).count();
    double total_estimated_seconds = elapsed_seconds / progress;
    double eta_seconds = total_estimated_seconds - elapsed_seconds;

    // Print ETA
    if (completed < total) {
        cout << "ETA: " << format_time(eta_seconds); // Display ETA in HH:MM:SS format
        cout << "\r"; // Carriage return to overwrite the line
    } else {
        cout << "Time Elapsed: " << format_time(elapsed_seconds); // Time taken after completion
        cout << "\u001b[0m\n\n"; // New line after completion
    }
    cout.flush(); // Ensure the output is displayed immediately
}

#endif // PROGRESSBAR_HPP

//
// Created by Mark Gagarine on 2025-11-19.
//

#ifndef CLIAPP_H
#define CLIAPP_H

class CliApp {
public:
    CliApp();
    ~CliApp();
    void run();

private:
    void runBasicInteractive();
    void runSimulatedInteractive();
    void runMonteCarlo();
};
#endif //CLIAPP_H

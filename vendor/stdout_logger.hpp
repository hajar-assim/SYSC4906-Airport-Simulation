/**
 * STDOUT logger for Cadmium v2.
 * Outputs simulation logs to standard output in CSV-like format.
 */

#ifndef CADMIUM_STDOUT_LOGGER_HPP_
#define CADMIUM_STDOUT_LOGGER_HPP_

#include <iostream>
#include <string>
#include <cadmium/core/logger/logger.hpp>

namespace cadmium {
    class STDOUTLogger : public Logger {
    private:
        std::string sep;
    public:
        explicit STDOUTLogger(std::string sep = ";") : Logger(), sep(std::move(sep)) {}

        void start() override {
            std::cout << "sep=" << sep << std::endl;
            std::cout << "time" << sep << "model_id" << sep << "model_name" << sep << "port_name" << sep << "data" << std::endl;
        }

        void stop() override {}

        void logOutput(double time, long modelId, const std::string& modelName, const std::string& portName, const std::string& output) override {
            std::cout << time << sep << modelId << sep << modelName << sep << portName << sep << output << std::endl;
        }

        void logState(double time, long modelId, const std::string& modelName, const std::string& state) override {
            std::cout << time << sep << modelId << sep << modelName << sep << sep << state << std::endl;
        }
    };
}

#endif //CADMIUM_STDOUT_LOGGER_HPP_

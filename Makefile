CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++11 -pthread
INCLUDES = -Iinclude -Ithird_party
BIN = bin

SRV_SRC = src/main_server.cpp \
          src/server/MainServer.cpp \
          src/server/Status.cpp \
          src/agg/Aggregator.cpp \
          src/repl/Replication.cpp src/repl/RaftLite.cpp \
          src/shard/ShardRouter.cpp src/shard/ShardRegistry.cpp \
          src/net/HttpServer.cpp src/net/HttpClient.cpp \
          src/types/TrafficReport.cpp src/types/RoadSnapshot.cpp \
          src/util/Logger.cpp src/util/Time.cpp src/util/JsonIO.cpp

SNS_SRC = src/main_sensor.cpp src/sensor/SensorRunner.cpp \
          src/net/HttpClient.cpp \
          src/types/TrafficReport.cpp \
          src/util/Time.cpp src/util/Logger.cpp src/util/JsonIO.cpp

.PHONY: all dirs clean server client

all: server client

dirs:
	@mkdir -p $(BIN)

server: dirs $(BIN)/server

$(BIN)/server: $(SRV_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

client: dirs $(BIN)/sensor

$(BIN)/sensor: $(SNS_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

clean:
	rm -rf $(BIN)

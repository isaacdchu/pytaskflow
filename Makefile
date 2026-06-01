run: main.py pytaskflow.cpp
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build build
	clear
	uv run main.py
debug: main.py pytaskflow.cpp
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
	cmake --build build
	clear
	uv run main.py
setup:
	@mkdir -p third_party
	@if [ -d third_party/taskflow ]; then \
		echo "third_party/taskflow already exists"; \
	else \
		git clone --depth 1 https://github.com/taskflow/taskflow.git third_party/taskflow_tmp && \
		if [ -d third_party/taskflow_tmp/taskflow ]; then \
			mv third_party/taskflow_tmp/taskflow third_party/taskflow; \
			rm -rf third_party/taskflow_tmp; \
		else \
			mv third_party/taskflow_tmp third_party/taskflow; \
		fi; \
	fi

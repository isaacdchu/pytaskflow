#include <nanobind/nanobind.h>
#include <taskflow/taskflow.hpp>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/function.h>

namespace nb = nanobind;
using namespace nb::literals;

std::vector<std::string> run_taskflow(std::string name = "default") {
    nb::print("Starting taskflow");
    nb::print(nb::str("{}").format(name));
    tf::Executor executor;
    tf::Taskflow taskflow;

    nb::ft_mutex mu;
    std::vector<std::string> outputs;

    auto [A, B, C, D] = taskflow.emplace(
        [&mu, &outputs]() {
            nb::ft_lock_guard lock(mu);
            outputs.push_back("TaskA");
        },
        [&mu, &outputs]() {
            nb::ft_lock_guard lock(mu);
            outputs.push_back("TaskB");
        },
        [&mu, &outputs]() {
            nb::ft_lock_guard lock(mu);
            outputs.push_back("TaskC");
        },
        [&mu, &outputs]() {
            nb::ft_lock_guard lock(mu);
            outputs.push_back("TaskD");
        }
    );
    A.precede(B, C);
    D.succeed(B, C);
    nb::print("Running taskflow");
    executor.run(taskflow).wait();
    return outputs;
}

NB_MODULE(pytaskflow, m) {
    m.def("run_taskflow",
        &run_taskflow,
        "name"_a = "default",
        "Run a simple taskflow example with the given name."
    );
    nb::class_<tf::Task>(m, "Task")
        .def(nb::init<>(),
            "Construct an empty Task that is not associated with any node in a Taskflow."
        )
        .def("name",
            static_cast<const std::string & (tf::Task::*)() const>(&tf::Task::name),
            "Return the name of the Task."
        )
        .def("name",
            static_cast<tf::Task & (tf::Task::*)(const std::string &)>(&tf::Task::name),
            "Set the name of the Task."
        )
        .def("precede",
            [](tf::Task &self, std::vector<tf::Task> tasks) {
                for (auto &task : tasks) {
                    self.precede(task);
                }
                return self;
            },
            "tasks"_a,
            "Set the successor tasks of this Task."
        )
        .def("precede",
            [](tf::Task &self, nb::args args) {
                for (auto h : args) {
                    tf::Task t = nb::cast<tf::Task>(h);
                    self.precede(t);
                }
                return self;
            },
            "tasks"_a,
            "Set the successor tasks of this Task."
        )
        .def("succeed",
            [](tf::Task &self, std::vector<tf::Task> tasks) {
                for (auto &task : tasks) {
                    self.succeed(task);
                }
                return self;
            },
            "tasks"_a,
            "Set the predecessor tasks of this Task."
        )
        .def("succeed",
            [](tf::Task &self, nb::args args) {
                for (auto h : args) {
                    tf::Task t = nb::cast<tf::Task>(h);
                    self.succeed(t);
                }
                return self;
            },
            "tasks"_a,
            "Set the predecessor tasks of this Task."
        )
        ;
    nb::class_<tf::Future<void>>(m, "Future")
        .def("cancel",
            &tf::Future<void>::cancel,
            "Cancel the execution of the running Taskflow associated with this Future."
        )
        .def("wait",
            &tf::Future<void>::wait,
            "Wait for the execution of the Taskflow associated with this Future to complete."
        )
        .def("get",
            &tf::Future<void>::get,
            "Wait for the execution of the Taskflow associated with this Future to complete and return the result."
        );
    nb::class_<tf::Taskflow>(m, "Taskflow")
        .def(nb::init<const std::string&>(),
            "name"_a = "",
            "Construct a Taskflow with the given name."
        )
        .def("name",
            static_cast<void (tf::Taskflow::*)(const std::string &)>(&tf::Taskflow::name),
            "Return the name of the Taskflow."
        )
        .def("name",
            static_cast<const std::string & (tf::Taskflow::*)() const>(&tf::Taskflow::name),
            "Set the name of the Taskflow."
        )
        .def("emplace",
            [](tf::Taskflow &self, std::function<void()> fn) {
                return self.emplace([fn]() { fn(); });
            },
            "Add a new task to the Taskflow."
        );
    nb::class_<tf::Executor>(m, "Executor")
        .def(nb::init<>(),
            "Construct an Executor with the default number of worker threads."
        )
        .def(nb::init<size_t>(),
            "N"_a,
            "Construct an Executor with N worker threads."
        )
        .def("run",
            static_cast<tf::Future<void> (tf::Executor::*)(tf::Taskflow&)>(&tf::Executor::run),
            "taskflow"_a,
            "Run a Taskflow and return a Future."
        )
        .def("run_n",
            static_cast<tf::Future<void> (tf::Executor::*)(tf::Taskflow&, size_t)>(&tf::Executor::run_n),
            "taskflow"_a,
            "N"_a,
            "Run a Taskflow N times and return a Future."
        );
}
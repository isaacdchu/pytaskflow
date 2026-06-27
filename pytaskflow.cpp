#include <nanobind/nanobind.h>
#include <taskflow/taskflow.hpp>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/function.h>

namespace nb = nanobind;
using namespace nb::literals;

NB_MODULE(pytaskflow, m) {
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
        );
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
    nb::class_<tf::FlowBuilder>(m, "FlowBuilder")
        .def("emplace",
            [](tf::FlowBuilder &self, std::function<void()> f) {
                std::function<void()> cb = [f]() { f(); };
                return self.emplace(cb);
            },
            "callable"_a,
            "Create a static task with the given callable target."
        )
        .def("emplace_runtime",
            [](tf::FlowBuilder &self, std::function<void(tf::Runtime&)> f) {
                std::function<void(tf::Runtime&)> cb = [f](tf::Runtime& r) { f(r); };
                return self.emplace(cb);
            },
            "callable"_a,
            "Create a runtime task with the given callable target."
        )
        .def("emplace_subflow",
            [](tf::FlowBuilder &self, nb::object fn) {
                return self.emplace([fn](tf::Subflow &sf) {
                    nb::gil_scoped_acquire acq;
                    nb::object py_sf = nb::cast(&sf);
                    fn(py_sf);
                });
            },
            "callable"_a,
            "Create a dynamic/subflow task with the given callable target."
        )
        .def("emplace_condition",
            [](tf::FlowBuilder &self, std::function<int()> f) {
                std::function<int()> cb = [f]() { return f(); };
                return self.emplace(cb);
            },
            "callable"_a,
            "Create a condition task with the given callable target."
        );
    nb::class_<tf::Subflow, tf::FlowBuilder>(m, "Subflow");
    nb::class_<tf::Taskflow, tf::FlowBuilder>(m, "Taskflow")
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
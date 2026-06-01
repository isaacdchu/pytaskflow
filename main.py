import sys
from build import pytaskflow as ptf

def main():
    print(f"Free threading enabled: {not sys._is_gil_enabled()}") # pylint: disable=protected-access
    executor = ptf.Executor(11)
    taskflow = ptf.Taskflow()
    taskflow.name("My Taskflow")
    t1 = taskflow.emplace(lambda: print("Task 1")).name("Task 1")
    t2 = taskflow.emplace(lambda: print("Task 2")).name("Task 2")
    t3 = taskflow.emplace(lambda: print("Task 3")).name("Task 3")
    t4 = taskflow.emplace(lambda: print("Task 4")).name("Task 4")
    t1.precede(t2, t3)
    t4.succeed(t2, t3)
    future = executor.run(taskflow)
    future.wait()
    print("Taskflow execution completed.")

if __name__ == "__main__":
    main()

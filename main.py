import sys
from build import pytaskflow as ptf

def simple():
    print("Running simple taskflow example...")
    executor = ptf.Executor()
    taskflow = ptf.Taskflow()
    taskflow.name("My Taskflow")
    t1 = taskflow.emplace(lambda: print("Task A")).name("Task A")
    t2 = taskflow.emplace(lambda: print("Task B")).name("Task B")
    t3 = taskflow.emplace(lambda: print("Task C")).name("Task C")
    t4 = taskflow.emplace(lambda: print("Task D")).name("Task D")
    t1.precede(t2, t3)
    t4.succeed(t2, t3)
    future = executor.run(taskflow)
    future.wait()
    print("Simple taskflow example completed.")

def subflow():
    print("Running subflow example...")
    executor = ptf.Executor()
    taskflow = ptf.Taskflow()
    A = taskflow.emplace(lambda: print("Task A")).name("A")
    C = taskflow.emplace(lambda: print("Task C")).name("C")
    D = taskflow.emplace(lambda: print("Task D")).name("D")
    def subflow_fn(sf: ptf.Subflow) -> None:
        B1 = sf.emplace(lambda: print("Task B1")).name("B1")
        B2 = sf.emplace(lambda: print("Task B2")).name("B2")
        B3 = sf.emplace(lambda: print("Task B3")).name("B3")
        B3.succeed(B1, B2)
    B = taskflow.emplace_subflow(subflow_fn).name("B")
    A.precede(B, C)
    D.succeed(B, C)
    future = executor.run(taskflow)
    future.wait()
    print("Subflow example completed.")

def condition():
    print("Running condition example...")
    import random
    executor = ptf.Executor()
    taskflow = ptf.Taskflow()
    A = taskflow.emplace(lambda: print("Task A")).name("A")
    B = taskflow.emplace(lambda: print("Task B")).name("B")
    def condition_fn() -> int:
        print("Evaluating condition...")
        return random.randint(0, 1)
    cond = taskflow.emplace_condition(condition_fn).name("Condition")
    A.precede(cond)
    cond.precede(cond, B)
    future = executor.run(taskflow)
    future.wait()
    print("Condition example completed.")

def main():
    print(f"Free threading enabled: {not sys._is_gil_enabled()}") # pylint: disable=protected-access
    simple()
    subflow()
    condition()

if __name__ == "__main__":
    main()

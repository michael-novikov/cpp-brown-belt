#include <iostream>
#include <string>
#include <tuple>
#include <map>

using namespace std;

#ifdef Dev

enum class TaskStatus {
  NEW,
  IN_PROGRESS,
  TESTING,
  DONE
};

using TasksInfo = map<TaskStatus, int>;

#endif

class TeamTasks {
public:
  const TasksInfo& GetPersonTasksInfo(const string& person) const {
    return person_tasks_info.at(person);
  }

  void AddNewTask(const string& person) {
    ++person_tasks_info[person][TaskStatus::NEW];
  }

  tuple<TasksInfo, TasksInfo> PerformPersonTasks(
        const string& person, int task_count) {
    TasksInfo& tasks_info = person_tasks_info[person];
    TasksInfo new_tasks_info;

    TasksInfo updated;
    TasksInfo untouched;

    auto status_order = {
      TaskStatus::NEW,
      TaskStatus::IN_PROGRESS,
      TaskStatus::TESTING
    };

    for (auto status : status_order) {
      int to_perform = min(tasks_info[status], task_count);
      int rest = tasks_info[status] - to_perform;

      if (to_perform) {
        TaskStatus next = static_cast<TaskStatus>(static_cast<int>(status) + 1);
        new_tasks_info[next] += to_perform;
        updated[next] = to_perform;

        task_count -= to_perform;
      }

      if (rest) {
        new_tasks_info[status] += rest;
        untouched[status] = rest;
      }
    }

    if (tasks_info.count(TaskStatus::DONE)) {
      new_tasks_info[TaskStatus::DONE] += tasks_info[TaskStatus::DONE];
    }

    tasks_info = new_tasks_info;

    return {updated, untouched};
  }
private:
  map<string, TasksInfo> person_tasks_info;
};

void PrintTasksInfo(TasksInfo tasks_info) {
  cout << tasks_info[TaskStatus::NEW] << " new tasks" <<
    ", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress" <<
    ", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested" <<
    ", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
  TeamTasks tasks;
  tasks.AddNewTask("Ilia");
  for (int i = 0; i < 3; ++i) {
  tasks.AddNewTask("Ivan");
  }
  cout << "Ilia's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
  cout << "Ivan's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

  TasksInfo updated_tasks, untouched_tasks;

  tie(updated_tasks, untouched_tasks) =
  tasks.PerformPersonTasks("Ivan", 2);
  cout << "Updated Ivan's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Ivan's tasks: ";
  PrintTasksInfo(untouched_tasks);

  tie(updated_tasks, untouched_tasks) =
  tasks.PerformPersonTasks("Ivan", 2);
  cout << "Updated Ivan's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Ivan's tasks: ";
  PrintTasksInfo(untouched_tasks);

return 0;
}


import matplotlib.pyplot as plt
import csv

def read_csv(filename):
    num_proc = []
    exec_time = []
    final_cost = []
    
    with open(filename, 'r') as file:
        reader = csv.DictReader(file)
        for row in reader:
            num_proc.append(int(row["num_proc"]))
            exec_time.append(float(row["avg_exec_time"]))
            final_cost.append(float(row["avg_final_cost"]))
    
    return num_proc, exec_time, final_cost

def plot_metrics(num_proc, exec_time, final_cost):
    plt.figure(figsize=(10, 8))
    plt.subplot(2, 1, 1)
    plt.plot(num_proc, exec_time, marker='o', linestyle='-', color='b')
    plt.title("Execution Time by Number of Processes")
    plt.xlabel("Number of Processes")
    plt.ylabel("Execution Time (seconds)")
    plt.grid(True)
    plt.subplot(2, 1, 2)
    plt.plot(num_proc, final_cost, marker='o', linestyle='-', color='g')
    plt.title("Final Cost by Number of Processes")
    plt.xlabel("Number of Processes")
    plt.ylabel("Final Cost")
    plt.grid(True)

    plt.tight_layout()
    plt.savefig("graph/metrics_plot.png", format="png")

def main():
    filename = "results_mult.csv"
    num_proc, exec_time, final_cost = read_csv(filename)
    plot_metrics(num_proc, exec_time, final_cost)

if __name__ == "__main__":
    main()

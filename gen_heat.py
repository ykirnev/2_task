import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt


def generate_heatmaps(file_path="results.csv"):
    df = pd.read_csv(file_path)
    cooling_methods = df["cooling_method"].unique()

    for method in cooling_methods:
        method_data = df[df["cooling_method"] == method]
        heatmap_data_execution = method_data.pivot_table(index="num_processors", columns="num_jobs",
                                                         values="execution_time", aggfunc='mean')
        plt.figure(figsize=(10, 8))
        sns.heatmap(heatmap_data_execution, annot=True, fmt=".2f", cmap="YlOrRd",
                    cbar_kws={'label': 'Execution Time (s)'})
        plt.gca().invert_yaxis() 
        plt.title(f"Heatmap of Execution Time for {method.capitalize()} Cooling")
        plt.xlabel("Number of Jobs")
        plt.ylabel("Number of Processors")
        plt.tight_layout()
        plt.savefig(f"graph/{method}_cooling_heatmap_execution_time.png")
        plt.close()
        print(f"Heatmap for {method} cooling method saved as {method}_cooling_heatmap_execution_time.png")
        heatmap_data_final_cost = method_data.pivot_table(index="num_processors", columns="num_jobs",
                                                          values="final_cost", aggfunc='mean')

        plt.figure(figsize=(10, 8))
        sns.heatmap(heatmap_data_final_cost, annot=True, fmt=".2f", cmap="Blues", cbar_kws={'label': 'Final Cost'})
        plt.gca().invert_yaxis() 
        plt.title(f"Heatmap of Final Cost for {method.capitalize()} Cooling")
        plt.xlabel("Number of Jobs")
        plt.ylabel("Number of Processors")
        plt.tight_layout()

        plt.savefig(f"graph/{method}_cooling_heatmap_final_cost.png")
        plt.close()
        print(f"Heatmap for {method} cooling method saved as graph/{method}_cooling_heatmap_final_cost.png")


if __name__ == "__main__":
    generate_heatmaps("results.csv")

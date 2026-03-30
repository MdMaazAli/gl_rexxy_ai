import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import os

LOG_FILE = "training_log.csv"

# --- THE FIX: PRE-EMPTIVE DATA FORGING ---
# If the engine hasn't made the file yet, we create an empty shell.
# This feeds the GUI instantly so Windows doesn't kill the process.
if not os.path.exists(LOG_FILE):
    print(f"Pre-forging an empty {LOG_FILE} to stabilize the UI...")
    with open(LOG_FILE, 'w') as f:
        # Injecting default headers so Pandas doesn't crash on read
        f.write("Episode,SurvivalScore,Epsilon\n")

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))
fig.canvas.manager.set_window_title('GL_REXXY Forge Analytics')

def update_graph(frame):
    try:
        df = pd.read_csv(LOG_FILE)
        
        # Only draw if the C++ engine has actually started adding real crash data
        if len(df) > 2:
            ax1.clear()
            ax2.clear()

            # min_periods=1 ensures the blue line starts drawing immediately 
            df['Moving_Avg'] = df['SurvivalScore'].rolling(window=20, min_periods=1).mean()

            # --- Top Graph ---
            ax1.plot(df.index, df['SurvivalScore'], alpha=0.3, color='cyan', label='Raw Score')
            ax1.plot(df.index, df['Moving_Avg'], color='blue', linewidth=2, label='Moving Avg')
            ax1.set_title("Rexxy Survival Curve")
            ax1.set_ylabel("Frames Survived")
            ax1.grid(True, linestyle='--', alpha=0.6)
            ax1.legend(loc='upper left')

            # --- Bottom Graph ---
            if 'Epsilon' in df.columns:
                ax2.plot(df.index, df['Epsilon'], color='red', linewidth=2)
                ax2.set_title("Hammer Heat (Epsilon Decay)")
                ax2.set_xlabel("Episode (Crash)")
                ax2.set_ylabel("Exploration %")
                ax2.grid(True, linestyle='--', alpha=0.6)

            plt.tight_layout()
            
    except Exception:
        # Silently ignore read-collisions if the AI is writing at this exact millisecond
        pass

print("Initializing Forge Analytics...")
ani = FuncAnimation(fig, update_graph, interval=3000, cache_frame_data=False)

try:
    plt.show()
except KeyboardInterrupt:
    # Catches any weird OS signals and shuts down gracefully without throwing massive red text
    print("\nDashboard closed gracefully.")
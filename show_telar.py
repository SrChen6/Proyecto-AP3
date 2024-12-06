import matplotlib.pyplot as plt
import matplotlib.patches as patches

def show_telar(rectangles):
    fig, ax = plt.subplots()
    for i, rect in enumerate(rectangles):
        xtl, ytl, xbr, ybr = rect
        # Ensure width and height are positive
        width = xbr - xtl + 1
        height = ybr - ytl + 1 
        if width > 0 and height > 0:
            colors = ['blue', 'green', 'red', 'yellow', 'purple', 'orange', 'brown', 'pink', 'gray', 'cyan']
            rect_patch = patches.Rectangle((xtl, ytl), width, height, linewidth=1.5, edgecolor='black', facecolor=colors[i % len(colors)])
            ax.add_patch(rect_patch)
        else:
            print(f"Invalid rectangle dimensions for {rect}: width and height must be positive.")
    # Set limits based on the rectangles with margins
    x_min = min(xtl for xtl, ytl, xbr, ybr in rectangles) - 1
    x_max = max(xbr for xtl, ytl, xbr, ybr in rectangles) + 2
    y_max = max(ybr for xtl, ytl, xbr, ybr in rectangles) + 1
    
    ax.set_xlim(x_min, x_max)
    ax.set_ylim(0, y_max)
    print(x_max)
    # Set ticks on the x and y axes
    ax.set_xticks(range(x_min, x_max + 1))  # Set x ticks
    ax.set_yticks(range(0, y_max + 1))      # Set y ticks

    # New code to add a light grid
    ax.grid(True, which='both', linestyle=':', linewidth=0.75, color='black')

    # New code to add vertical lines at x = 0 and x = x_max - 1
    ax.axvline(x=0, color='black', linestyle='-', linewidth=2)  # Vertical line at x = 0
    ax.axvline(x=x_max - 1, color='black', linestyle='-', linewidth=2)  # Vertical line at x = x_max - 1

    plt.gca().set_aspect('equal', adjustable='box')  # Keep aspect ratio
    plt.show()

# Example usage:
with open('output_exh.txt', 'r') as file:
    rectangles = [tuple(map(int, line.split())) for line in file.readlines()[2:]]
show_telar(rectangles)

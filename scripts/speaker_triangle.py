import math

def calculate_angles(base, height):
    # Calculate the angle at the base using the tangent function
    theta = math.degrees(math.atan(2 * height / base))

    # Calculate the top angle
    alpha = 180 - 2 * theta

    return theta, alpha

# Example usage
base = float(input("Enter the base (distance from left to right): "))
height = float(input("Enter the height (from bottom line to top point): "))

theta, alpha = calculate_angles(base, height)

print(f"The angles at the base are: {theta:.2f} degrees each")
print(f"The angle at the top is: {alpha:.2f} degrees")
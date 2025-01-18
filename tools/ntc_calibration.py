#!/usr/bin/env python3

import sys
import numpy as np
import pandas as pd

def main():
    if len(sys.argv) < 3:
        print("Usage: python steinhart_calibration.py <datafile.csv> <const_prefix>")
        sys.exit(1)

    # Read the CSV file
    csv_path = sys.argv[1]
    const_prefix = sys.argv[2]
    data = pd.read_csv(csv_path)

    # Extract temperature (K) and resistance (Ohms) columns
    T = data['Temperature'].values
    R = data['Resistance'].values

    # Build the target vector y = 1 / T
    y = 1.0 / (T + 273.15) 

    # Build the design matrix for x = [1, ln(R), (ln(R))^3]
    lnR = np.log(R)
    X = np.column_stack([np.ones(len(R)), lnR, lnR**3])

    # Solve the linear system in the least-squares sense:
    # y = A0 + A1 * ln(R) + A2 * (ln(R))^3
    # => [A0, A1, A2]
    coeffs, residuals, rank, s = np.linalg.lstsq(X, y, rcond=None)
    A0, A1, A2 = coeffs

    y_hat = X @ coeffs
    ss_res = np.sum((y - y_hat)**2)          # residual sum of squares
    ss_tot = np.sum((y - np.mean(y))**2)     # total sum of squares
    r2 = 1 - ss_res / ss_tot 

    # Print the results
    print("//NTC parameters for {const_prefix}")
    print(f"#define {const_prefix}_ALPHA {A0}f")
    print(f"#define {const_prefix}_BETA {A1}f")
    print(f"#define {const_prefix}_GAMMA {A2}f")

    # Optionally, you can print the residuals to see how good the fit is
    # (residuals is a 1-element array if there are more data points than parameters)
    if len(residuals) > 0:
        print(f"\nCoefficient of Determination (R²): {r2:.6f}")

if __name__ == "__main__":
    main()

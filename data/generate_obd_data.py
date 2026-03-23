import csv
import random

INPUT_FILE = "2017-07-05_Seat_Leon_RT_S_Stau.csv"
OUTPUT_FILE = "dataset.csv"

IN_SPEED = "Vehicle Speed Sensor [km/h]"
IN_ENGINE_RPM = "Engine RPM [RPM]"
IN_THROTTLE_POS = "Absolute Throttle Position [%]"
IN_COOL_TEMP = "Engine Coolant Temperature [Â°C]"
IN_FUEL_LEVEL = ""
IN_INTAKE_AIR_TEMP = "Intake Air Temperature [Â°C]"
IN_LABEL = ""

OUT_SPEED = "speed_kmh"
OUT_ENGINE_RPM = "engine_rpm"
OUT_THROTTLE_POS = "throttle_pos"
OUT_COOL_TEMP = "coolant_temp"
OUT_FUEL_LEVEL = "fuel_level"
OUT_INTAKE_AIR_TEMP = "intake_air_temp"
OUT_LABEL = "label"

def classify_label(speed, rpm, throttle):
    if rpm < 2000 and speed < 60 and throttle < 30:
        return "SLOW"
    elif rpm < 4000 and speed < 100:
        return "NORMAL"
    else:
        return "AGGRESSIVE"

def classify_fuel_level(fuel_level, speed, rpm, throttle):
    fuel_rate = (
                0.01 * speed +
                0.005 * rpm / 1000 +
                0.02 * throttle / 100
            )

    fuel_rate *= random.uniform(0.9, 1.1)

    fuel_level -= fuel_rate
    fuel_level = max(fuel_level, 0.0)
    return fuel_level

def main():
    row_count = 0
    with open(INPUT_FILE, "r", newline="") as infile, \
         open(OUTPUT_FILE, "w", newline="") as outfile:

        output_fields = [
            "speed_kmh",
            "engine_rpm",
            "throttle_pos",
            "coolant_temp",
            "fuel_level",
            "intake_air_temp",
            "label"
        ]
        writer = csv.DictWriter(outfile, fieldnames=output_fields)
        writer.writeheader()

        reader = csv.DictReader(infile)
        input_fields = reader.fieldnames.copy()

        fuel_level = 100.0
        for input_row in reader:
            try:
                # speed_kmh
                if IN_SPEED in input_fields: 
                    speed_kmh = float(input_row.get(IN_SPEED))
                else:
                    speed_kmh = 0
                # engine_rpm
                if IN_ENGINE_RPM in input_fields: 
                    engine_rpm = float(input_row.get(IN_ENGINE_RPM))
                else:
                    engine_rpm = 0
                # throttle_pos
                if IN_THROTTLE_POS in input_fields: 
                    throttle_pos = float(input_row.get(IN_THROTTLE_POS))
                else:
                    throttle_pos = 0
                # coolant_temp
                if IN_COOL_TEMP in input_fields: 
                    coolant_temp = float(input_row.get(IN_COOL_TEMP))
                else:
                    coolant_temp = 0
                # fuel_level
                if IN_FUEL_LEVEL in input_fields: 
                    fuel_level = float(input_row.get(IN_FUEL_LEVEL))
                else:
                    fuel_level = classify_fuel_level(fuel_level, speed_kmh, engine_rpm, throttle_pos)
                # intake_air_temp
                if IN_INTAKE_AIR_TEMP in input_fields: 
                    intake_air_temp = float(input_row.get(IN_INTAKE_AIR_TEMP))
                else:
                    intake_air_temp = 0
                # label
                if IN_LABEL in input_fields:
                    label = float(input_row.get(IN_LABEL))
                else:
                    label = classify_label(speed_kmh, engine_rpm, throttle_pos)

            except ValueError:
                continue

            output_row = {
                OUT_SPEED: round(speed_kmh),
                OUT_ENGINE_RPM: round(engine_rpm, 2),
                OUT_THROTTLE_POS: round(throttle_pos, 2),
                OUT_COOL_TEMP: round(coolant_temp, 2),
                OUT_FUEL_LEVEL: round(fuel_level, 2),
                OUT_INTAKE_AIR_TEMP: round(intake_air_temp, 2),
                OUT_LABEL: label
            }

            writer.writerow(output_row)
            row_count += 1
            if(row_count > 5000):
                break

if __name__ == "__main__":
    main()

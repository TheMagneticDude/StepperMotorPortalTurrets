from music21 import converter, note, chord, stream
import os

def midi_pitch_to_freq(midi_num):
    return round(440 * 2 ** ((midi_num - 69) / 12))

def duration_to_denominator(qLen):
    base = {4.0: 1, 2.0: 2, 1.0: 4, 0.5: 8, 0.25: 16}
    for dur, denom in base.items():
        if abs(qLen - dur * 1.5) < 0.01:
            return denom, 'DOTTED'
        if abs(qLen - dur) < 0.01:
            return denom, None
    return 4, None

def velocity_to_flags(velocity):
    flags = []
    if velocity < 40:
        flags.append("STACCATO")
    elif velocity > 100:
        flags.append("VIBRATO_MED")
    return flags

def convert_midi_voice_aware_with_measures(file_path, num_steppers, output_dir="stepper_output"):
    score = converter.parse(file_path)
    steppers = [[] for _ in range(num_steppers)]
    stepper_times = [0.0 for _ in range(num_steppers)]
    measure_offsets = set()

    # Assign primary steppers to each part
    primary_assignments = {}
    next_stepper = 0

    for part_index, part in enumerate(score.parts):
        assigned_stepper = next_stepper % num_steppers
        next_stepper += 1
        primary_assignments[part] = assigned_stepper

        # Gather all notes with their offsets and measures
        notes_by_offset = {}
        for measure in part.getElementsByClass(stream.Measure):
            offset = float(measure.offset)
            measure_offsets.add((offset, measure.number))

            for element in measure.notes:
                if isinstance(element, note.Note):
                    notes_by_offset.setdefault(element.offset, []).append(element)
                elif isinstance(element, chord.Chord):
                    for n in element.notes:
                        notes_by_offset.setdefault(element.offset, []).append(n)

        # Sort and process all notes by time
        for offset in sorted(notes_by_offset):
            notes = notes_by_offset[offset]
            for i, n in enumerate(notes):
                assigned = (primary_assignments[part] + i) % num_steppers

                # Insert rest if time has passed
                current_time = stepper_times[assigned]
                if offset > current_time:
                    rest_duration = offset - current_time
                    denom, dotted_flag = duration_to_denominator(rest_duration)
                    flags = ["DOTTED"] if dotted_flag else []
                    flags_str = " | ".join(flags) if flags else "0"
                    steppers[assigned].append(f"{{ 0, {denom}, {flags_str} }},  // rest")
                    stepper_times[assigned] = offset

                # Add the note
                freq = midi_pitch_to_freq(n.pitch.midi)
                note_duration = n.duration.quarterLength
                denom, dotted_flag = duration_to_denominator(note_duration)
                flags = velocity_to_flags(n.volume.velocity or 64)
                if dotted_flag:
                    flags.append("DOTTED")
                flags_str = " | ".join(flags) if flags else "0"
                steppers[assigned].append(
                    f"{{ {freq}, {denom}, {flags_str} }},  // part {part_index+1}, offset {offset}, note {n.nameWithOctave}"
                )
                stepper_times[assigned] += note_duration

    # Insert // Measure X labels at the correct times in each stepper
    for offset, measure_num in sorted(measure_offsets):
        for i in range(num_steppers):
            if stepper_times[i] < offset:
                rest_duration = offset - stepper_times[i]
                denom, dotted_flag = duration_to_denominator(rest_duration)
                flags = ["DOTTED"] if dotted_flag else []
                flags_str = " | ".join(flags) if flags else "0"
                steppers[i].append(f"{{ 0, {denom}, {flags_str} }},  // rest")
                stepper_times[i] = offset
            steppers[i].append(f"// Measure {measure_num}")

    # Align ending
    max_time = max(stepper_times)
    for i in range(num_steppers):
        if stepper_times[i] < max_time:
            rest_duration = max_time - stepper_times[i]
            denom, dotted_flag = duration_to_denominator(rest_duration)
            flags = ["DOTTED"] if dotted_flag else []
            flags_str = " | ".join(flags) if flags else "0"
            steppers[i].append(f"{{ 0, {denom}, {flags_str} }},  // rest to align ending")

    # Write to files
    os.makedirs(output_dir, exist_ok=True)
    for i, track in enumerate(steppers):
        filename = os.path.join(output_dir, f"stepper{i+1}.txt")
        with open(filename, "w") as f:
            for line in track:
                f.write(line + "\n")
        print(f"Wrote {filename}")

# === Example usage ===
convert_midi_voice_aware_with_measures("Time_To_Say_Goodbye.mid", num_steppers=3)

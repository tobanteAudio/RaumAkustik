import re

import librosa
import librosa.display
import matplotlib.pyplot as plt
import numpy as np


def is_header_line(line):
    return line.startswith('*')


def parse_header_line(line):
    return line.strip('* ').strip()


def parse_data_line(line):
    splits = line.split(',')
    return float(splits[0]), float(splits[1]), float(splits[2])


def parse_rew_version(line):
    return re.search(r'([\d.]+)', line).group(1)


def split_header_and_data(content):
    header = []
    data = []
    for line in content.splitlines():
        if is_header_line(line):
            header.append(parse_header_line(line))
        else:
            data.append(line)
    return header, data


def main():
    file_path = 'scripts/test_data/measurement_192k.txt'
    with open(file_path) as file:
        header_lines, data_lines = split_header_and_data(file.read())
        for attribute in header_lines:
            print(attribute)

        print(parse_rew_version(header_lines[0]))

        freq = []
        spl = []
        phase = []
        for line in data_lines:
            f, s, p = parse_data_line(line)
            freq.append(f)
            spl.append(s)
            phase.append(p)

        # plt.figure()
        # plt.plot(freq, spl)
        # plt.plot(freq, phase)
        # plt.xlabel('Frequency')
        # plt.ylabel('SPL [dB]')
        # plt.xlim([20, 20000])
        # plt.ylim([50, 100])
        # plt.xscale('log')
        # plt.grid()
        # plt.show()

        fig, spl_ax = plt.subplots()
        fig.title = 'SPL'

        color = 'tab:red'
        # spl_ax.xaxis.set_major_formatter(librosa.display.NoteFormatter())
        spl_ax.xaxis.set_major_formatter(librosa.display.LogHzFormatter())
        spl_ax.set(ylabel='Hz', xlabel='SPL')
        spl_ax.set_xlim([30, 300])
        spl_ax.set_ylim([50, 100])
        spl_ax.plot(freq, spl, color=color)

        # phase_ax = spl_ax.twinx()  # instantiate a second axes that shares the same x-axis

        # color = 'tab:blue'
        # phase_ax.set_ylabel('Phase (deg)', color=color)
        # phase_ax.plot(freq, phase, color=color)
        # phase_ax.tick_params(axis='y', labelcolor=color)

        # fig.tight_layout()  # otherwise the right y-label is slightly clipped
        plt.show()


main()

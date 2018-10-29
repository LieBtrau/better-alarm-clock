#include "secondsDecoder.h"

SecondsDecoder::SecondsDecoder()
{
    memset(_data, 0, sizeof(_data));
}

void SecondsDecoder::updateSeconds(const PULSE_TYPES tick_data)
{
    // We use a binning approach to find out the proper phase.
    // The goal is to localize the sync_mark. Due to noise
    // there may be wrong marks of course. The idea is to not
    // only look at the statistics of the marks but to exploit
    // additional data properties:

    // Bit position  0 after a proper sync is a 0.
    // Bit position 20 after a proper sync is a 1.

    // The binning will work as follows:

    //   1) A sync mark will score +6 points for the current bin
    //      it will also score -2 points for the previous bin
    //                         -2 points for the following bin
    //                     and -2 points 20 bins later
    //  In total this will ensure that a completely lost signal
    //  will not alter the buffer state (on average)

    //   2) A 0 will score +1 point for the previous bin
    //      it also scores -2 point 20 bins back
    //                 and -2 points for the current bin

    //   3) A 1 will score +1 point 20 bins back
    //      it will also score -2 point for the previous bin
    //                     and -2 points for the current bin

    //   4) An undefined value will score -2 point for the current bin
    //                                    -2 point for the previous bin
    //                                    -2 point 20 bins back

    //   5) Scores have an upper limit of 255 and a lower limit of 0.

    // Summary: sync mark earns 6 points, a 0 in position 0 and a 1 in position 20 earn 1 bonus point
    //          anything that allows to infer that any of the "connected" positions is not a sync will remove 2 points

    // It follows that the score of a sync mark (during good reception)
    // may move up/down the whole scale in slightly below 64 minutes.
    // If the receiver should glitch for whatever reason this implies
    // that the clock will take about 33 minutes to recover the proper
    // phase (during phases of good reception). During bad reception things
    // are more tricky.

    const uint8_t previous_tick = _activeBin > 0 ? _activeBin - 1 : SECONDS_PER_MINUTE - 1;
    const uint8_t previous_21_tick = _activeBin > 20 ? _activeBin - 21 : _activeBin + SECONDS_PER_MINUTE - 21;
    const uint8_t next_tick = _activeBin < SECONDS_PER_MINUTE - 1 ? _activeBin + 1 : 0;

    switch (tick_data)
    {
    case SYNC_MARK:
        bounded_increment(_data[_activeBin], 6);
        bounded_decrement(_data[previous_tick], 2);
        bounded_decrement(_data[previous_21_tick], 2);
        bounded_decrement(_data[next_tick], 2);
        break;
    case SHORT_PULSE:
        bounded_increment(_data[previous_tick], 1);
        bounded_decrement(_data[_activeBin], 2);
        bounded_decrement(_data[previous_21_tick], 2);
        break;
    case LONG_PULSE:
        bounded_increment(_data[previous_21_tick], 1);
        bounded_decrement(_data[_activeBin], 2);
        bounded_decrement(_data[previous_tick], 2);
        break;
    case UNDEFINED:
    default:
        bounded_decrement(_data[_activeBin], 2);
        bounded_decrement(_data[previous_tick], 2);
        bounded_decrement(_data[previous_21_tick], 2);
    }
    //Advance bin
    _activeBin = _activeBin < SECONDS_PER_MINUTE - 1 ? _activeBin + 1 : 0;

    //Find sync mark bin
    _max = 0;
    _noise_max = 0;
    _max_index = 255;
    for (uint8_t index = 0; index < SECONDS_PER_MINUTE; ++index)
    {
        const uint8_t bin_data = _data[index];
        if (bin_data >= _max)
        {
            _noise_max = _max;
            _max = bin_data;
            _max_index = index;
        }
        else if (bin_data > _noise_max)
        {
            _noise_max = bin_data;
        }
    }
}

uint8_t SecondsDecoder::getSecond()
{
    if (_max - _noise_max >= LOCK_THRESHOLD)
    {
        // at least one sync mark and a 0 and a 1 seen
        // the threshold is tricky:
        //   higher --> takes longer to acquire an initial lock, but higher probability of an accurate lock
        //
        //   lower  --> higher probability that the lock will oscillate at the beginning
        //              and thus spoil the downstream stages

        // we have to subtract 2 seconds
        //   1 because the seconds already advanced by 1 tick
        //   1 because the sync mark is not second 0 but second 59
        uint8_t second = 2 * SECONDS_PER_MINUTE + _activeBin - 2 - _max_index;
        while (second >= SECONDS_PER_MINUTE)
        {
            second -= SECONDS_PER_MINUTE;
        }
        return second;
    }
    else
    {
        return 0xff;
    }
}

void SecondsDecoder::bounded_increment(uint8_t &value, uint8_t N)
{
    value = (value >= 255 - N) ? 255 : value + N;
}

void SecondsDecoder::bounded_decrement(uint8_t &value, uint8_t N)
{
    value = (value <= N) ? 0 : value - N;
}

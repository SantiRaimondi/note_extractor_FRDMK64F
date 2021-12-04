# note_extractor_FRDMK64F
The board extracts the notes of a song previously loaded in its flash memory.
---

We followed the same basic algorithm implemented in [Matlab by Nayiri Krzysztofowicz](https://www.youtube.com/watch?v=m7FuJaxWD3A). Also explained in [this paper](https://www.sciencedirect.com/science/article/pii/S1877050915020281).

The challenge we faced was doing the actual implementation on a Cortex-M4 MCU (we used the FRDMK64F board).

Needless to say there we cut some corners. For example, the threasholding was made on a PC, and the threasholded values were only uploaded to the  board. This was because it happened to be very computationally expensive to calculate the median (due to the inefficient sorting algorithms).

Here there is a [video](https://drive.google.com/file/d/1OAFdbchQEfe5q50G29sxiyNeZyh-o82t/view?usp=sharing) with our results. We also wrote a [paper](https://drive.google.com/file/d/10mIz4Rfz9Y-ysgHkM1ysG0we1GFYGZyE/view?usp=sharing) where we thoroughly explained what we did.

Helpful files are available in the *MusicNoteExtraction-modified* directory. Is a copy of Nayiri Krzysztofowicz repository but modified to save a *.h* file ready to be uploaded to the Cortex MCU.

# Folder with automation scripts for external tools

Scripts must be executed from **${projectRoot}** directory.

Used tools:

- **cmake/ctest**. Build generation system. Building and testing automation.
- **OpenOCD**. Debug issues and uploading/downloading firmaware on developement board.
- **stm32flash**. Communication with default stm32 bootloader.

**OpenOCD** and **stm32flash** must be placed in same directory like this project.

    ... projectsDir / ${projectRoot} / scripts / bash / ...
                                               / cmd / ...
                                               / googleTest / ...
                                               / stm32flash / ...
                                               / OpenOCD / ...

**cmake** must be added to **PATH**
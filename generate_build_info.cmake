string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP BUILD_TIME "%H:%M:%S")
string(TIMESTAMP BUILD_DATETIME "%Y-%m-%d %H:%M:%S")

# Print in cyan color
string(ASCII 27 ESC)
message(STATUS "${ESC}[36mBuild timestamp: ${BUILD_DATETIME}${ESC}[0m")

configure_file(
    "${INPUT_FILE}"
    "${OUTPUT_FILE}"
    @ONLY
)

import React, {useEffect, useState} from 'react';
import ReactDiffViewer from 'react-diff-viewer';

function UnitTest({unit}) {

	const [activeDiff, setActiveDiff] = useState(null);
	const [copied, setCopied] = useState(false)

	const [output42sh, setOutput42sh] = useState(null);
	const [outputBash, setOutputBash] = useState(null);
	const [error42sh, setError42sh] = useState(null);
	const [errorBash, setErrorBash] = useState(null);
	const [input, setInput] = useState(null);

	// Function to fetch file content from the backend API
	const fetchContent = async (filePath, setter) => {
		try {
			const encodedPath = encodeURIComponent(filePath);
			const url = `http://localhost:4000/api/files/${encodedPath}`
			const response = await fetch(url);
			console.log("Fetching from URL:", url);  // Log the full URL to debug
			if (!response.ok) {
				throw new Error(`Failed to fetch ${url}: ${response.statusText}`);
			}
			const fileContent = await response.json(); // Assuming the backend sends the file content as JSON
			setter(fileContent); // Set the content into the state
		} catch (err) {
			setter(`Error fetching file: ${err.message}`);
		}
	};

	// Fetch the content for input, output, and error files using the new API
	useEffect(() => {
		fetchContent(unit.input, setInput);
		fetchContent(unit["42sh_output"], setOutput42sh);
		fetchContent(unit["bash_output"], setOutputBash);
		fetchContent(unit["42sh_error"], setError42sh);
		fetchContent(unit["bash_error"], setErrorBash);
	}, [unit]);

	//unitButton style
	const unitButton = (unit, type) => {
		let buffer = "py-1 px-2 m-2 text-center border-2 rounded";
		if (type === "output" && unit["output_ok"] === "false")
			buffer += " bg-red-500";
		else if (type === "error" && unit["error_ok"] === "false")
			buffer += " bg-yellow-500";
		else if (type === "exit" && unit["exit_ok"] === "false")
			buffer += " bg-red-500";
		if (type === activeDiff)
			buffer += " border-green-700"
		return buffer;
	}

	const newStyles = {
		variables: {
			dark: {
				highlightBackground: '#fefed5',
				highlightGutterBackground: '#ffcd3c',
			},
		},
		line: {
			padding: '10px 2px',
			color: '#24292e',
		},
		titleBlock: {
			textAlign: 'center',
		},
		lineNumber: {
			textAlign: 'center',
		}
	}

	const setShown = (value) => {
		setActiveDiff((active) => (value === active) ? null : value);
	}

	const copyToClipboard = (text) => {
		navigator.clipboard.writeText(text)
		setCopied(true)
	}

	return (
		<>
			{input ? (
				<>
					<p 
						className="p-2 m-3 bg-zinc-800 rounded-md border-2 text-center text-xl border-red-300 cursor-pointer"
						onClick={() => copyToClipboard(input)}
					>
						{input} 
					</p>
					{copied && (<p>Copied !</p>)}
				</>
			) : (
				<p> loading... </p>
			)}

			<div className="flex-col buttonsContainer">
				<button className={unitButton(unit, "output")} 
					onMouseEnter={() => setShown("output")}
					onMouseLeave={() => setShown(null)}
				>
					Ouput (stdout)
				</button>
				<button className={unitButton(unit, "error")} 
					onMouseEnter={() => setShown("error")}
					onMouseLeave={() => setShown(null)}
				>
					Error (stderr)
				</button>
				<button className={unitButton(unit, "exit")} 
					onMouseEnter={() => setShown("exit")}
					onMouseLeave={() => setShown(null)}
				>
					Exit code ($?)
				</button>
			</div>
			{ activeDiff !== null &&
				<>
					<div className="rounded-md border">
						{activeDiff === "output" && 
							<ReactDiffViewer key={unit.id + "output"} 
								styles={newStyles}
								oldValue={outputBash || "None"} 
								newValue={output42sh || "None"} 
								splitView={true} showDiffOnly={false} 
								leftTitle={"Bash"} rightTitle={"42sh"} 
							/>
						}
						{activeDiff === "error" && 
							<ReactDiffViewer key={unit.id + "error"} 
								styles={newStyles}
								oldValue={errorBash || "None"} 
								newValue={error42sh || "None"} 
								splitView={true} showDiffOnly={false} 
								leftTitle={"Bash"} rightTitle={"42sh"} 
							/>
						}
						{activeDiff === "exit" &&
							<ReactDiffViewer key={unit.id + "output"} 
								styles={newStyles}
								oldValue={unit["42sh_exit_code"]} 
								newValue={unit["bash_exit_code"]} 
								splitView={true} showDiffOnly={false} 
								leftTitle={"Bash"} rightTitle={"42sh"} 
							/>
						}
					</div>
				</>
			}
		</>
	);
}

export default UnitTest;

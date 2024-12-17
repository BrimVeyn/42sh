import React, {useEffect, useState, useMemo} from 'react';
import ReactDiffViewer from 'react-diff-viewer';

function UnitTest({unit}) {

	const [activeDiff, setActiveDiff] = useState(null);

	const [output42sh, setOutput42sh] = useState(null);
	const [outputBash, setOutputBash] = useState(null);
	const [error42sh, setError42sh] = useState(null);
	const [errorBash, setErrorBash] = useState(null);
	const [input, setInput] = useState(null);

	//Function to statically fetch from ./public
	const fetchContent = async (filePath, setter) => {
		try {
			const response = await fetch(filePath);
			if (!response.ok) {
				throw new Error(`Failed to fetch ${filePath}: ${response.statusText}`);
			}
			const text = await response.text();
			setter(text);
		} catch (err) {
			setter(`Error fetching file: ${err.message}`);
		}
	};
	//Fetch input inconditionnaly
	fetchContent('/' + unit.input, setInput);
	fetchContent('/' + unit["42sh_output"], setOutput42sh);
	fetchContent('/' + unit["bash_output"], setOutputBash);
	fetchContent('/' + unit["42sh_error"], setError42sh);
	fetchContent('/' + unit["bash_error"], setErrorBash);

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

	const setShown = (setter, value) => {
		setter((active) => (value === active) ? null : value);
	}

	return (
		<>
			{input ? (
				<p className="p-2 m-3 bg-zinc-800 rounded-md border-2 text-center border-red-300"> 
					<strong>Input: </strong>
					{input} 
				</p>
			) : (
				<p> loading... </p>
			)}

			<div className="flex-col buttonsContainer">
				<button className={unitButton(unit, "output")} onClick={() => setShown(setActiveDiff, "output")}>
					Ouput (stdout)
				</button>
				<button className={unitButton(unit, "error")} onClick={() => setShown(setActiveDiff, "error")}>
					Error (stderr)
				</button>
				<button className={unitButton(unit, "exit")} onClick={() => setShown(setActiveDiff, "exit")}>
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

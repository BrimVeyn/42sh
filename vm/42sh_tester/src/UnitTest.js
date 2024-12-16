import React, {useEffect, useState} from 'react';

function UnitTest({unit}) {
	const [showOutput, setShowOutput] = useState(false);
	const [showError, setShowError] = useState(false);
	const [showExitCode, setShowExitCode] = useState(false);

	const [output42sh, setOutput42sh] = useState(null);
	const [outputBash, setOutputBash] = useState(null);
	const [error42sh, setError42sh] = useState(null);
	const [errorBash, setErrorBash] = useState(null);
	const [input, setInput] = useState(null);

	//useState toggler (bool)
	const toggle = (setShow) => setShow((prev) => !prev);
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
	const unitButton = "py-1 px-2 m-2 text-center border-2 rounded";

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
				<button className={unitButton} onClick={() => toggle(setShowOutput)}>
					Ouput (stdout)
				</button>
				<button className={unitButton} onClick={() => toggle(setShowError)}>
					Error (stderr)
				</button>
				<button className={unitButton} onClick={() => toggle(setShowExitCode)}>
					Exit code ($?)
				</button>
			</div>
			{ (showOutput || showError || showExitCode) &&
				<>
					<div className="p-4 m-4 rounded-md border">
						{showOutput && 
							<>
								<p><strong>42sh Output:</strong> {output42sh || "None"}</p>
								<p><strong>Bash Output:</strong> {outputBash || "None"}</p>
							</>
						}
						{showError && 
							<>
								<p><strong>42sh Error:</strong> {error42sh || "None"}</p>
								<p><strong>Bash Error:</strong> {errorBash || "None"}</p>
							</>
						}
						{showExitCode &&
							<>
								<p><strong>42sh ExitCode:</strong> {unit["42sh_exit_code"] || "None"}</p>
								<p><strong>Bash ExitCode:</strong> {unit["bash_exit_code"] || "None"}</p>
							</>
						}
					</div>
				</>
			}
		</>
	);
}

export default UnitTest;

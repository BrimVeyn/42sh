import React, {useState, useEffect} from 'react'
import UnitTest from './UnitTest'

function Resume({ tests }) {

	const [shownIndex, setShownIndex] = useState(null);

	const toggleTest = (index) => {
		setShownIndex((prevIndex) => (prevIndex === index ? null : index));
	}
	
	// console.log("shownIndex: ", shownIndex, tests.length);

	const computeBg = (test) => {
		if (test["output_ok"] === "false" || test["exit_ok"] === "false") return "bg-red-500"
		if (test["error_ok"] === "false") return "bg-yellow-500"
		return "bg-green-500"
	}
	const computeBorder = (index) => {
		if (index == shownIndex) return "border-green-700";
		return "";
	}

	return (
		<>
			<div className="w-3/4 text-center resumeContainer">
				{tests.map((test, index) => (
					<button
						key={index}
						className={`rounded border-2 p-1 m-1 resumeButton ${computeBg(test)} ${computeBorder(index)}`}
						onClick={() => toggleTest(index)}
					>
						{test.id}
					</button>
				))}
			</div>
			{shownIndex !== null && shownIndex < tests.length && (
				<UnitTest unit={tests[shownIndex]} />
			)}
		</>
	);
}

export default Resume;

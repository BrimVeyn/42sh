import React, {useState} from 'react'
import UnitTest from './UnitTest'

function Resume({ tests, title }) {

	const [shownIndex, setShownIndex] = useState(null);

	const toggleTest = (index) => {
		setShownIndex((prevIndex) => (prevIndex === index ? null : index));
	}
	
	const computeBg = (test) => {
		if (test["output_ok"] === "0" || test["exit_ok"] === "0") return "bg-red-500"
		if (test["error_ok"] === "0") return "bg-yellow-500"
		return "bg-green-500"
	}
	const computeBorder = (index) => {
		if (index === shownIndex) return "border-4 border-blue-500";
		return "";
	}

	return (
		<>
			<div id="resumeTitle" className="p-5 text-xl text-center text-gray-200" >
				<p> 
					{title.replace(/_/g, ' ').toUpperCase()} 
				</p>
			</div>
			<div className="flex flex-wrap m-3 w-3/4 text-center resumeContainer">
				{tests.map((test, index) => (
					<button
						key={index}
						className={`w-10 h-10 rounded border-2 p-1 m-1 resumeButton ${computeBg(test)} ${computeBorder(index)}`}
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

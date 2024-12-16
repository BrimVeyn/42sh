import React, {useState} from 'react'
import UnitTest from './UnitTest'

function Resume({ tests }) {

	const [shownIndex, setShownIndex] = useState(null);

	const toggleTest = (index) => {
		setShownIndex((prevIndex) => (prevIndex === index ? null : index));
	}

	console.log('Shown', shownIndex, tests[shownIndex]);
	return (
		<>
			<div className="w-3/4 text-center resumeContainer">
				{tests.map((test, index) => (
					<button 
						className="rounded border-2 border-green-700 p-1 m-1 resumeButton" 
						key={index} 
						onClick={() => toggleTest(index)}
					>
						{test.id}
					</button>
				))}
			</div>
			{shownIndex !== null && (
				<UnitTest unit={tests[shownIndex]} />
			)}
		</>
	);
}

export default Resume;

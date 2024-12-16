import React, {useState} from 'react'
import "./Resume.css"
import UnitTest from './UnitTest'

function Resume({ tests }) {

	const [shownIndex, setShownIndex] = useState(null);

	const toggleTest = (index) => {
		setShownIndex((prevIndex) => (prevIndex === index ? null : index));
	}

	return (
		<>
			<div className="resumeContainer">
				{tests.map((test, index) => (
					<button 
						className="resumeButton" 
						key={index} 
						onClick={() => toggleTest(index)}
					>
						{test.id}
					</button>
				))}
			</div>
			{shownIndex && (
				<UnitTest unit={tests[shownIndex]} />
			)}
		</>
	);
}

export default Resume;

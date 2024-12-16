import React, {useState} from 'react';
import UnitTest from './UnitTest';
import Resume from './Resume';
import "./Category.css"

function Category({name, tests}) {

	const [showResume, setShowResume] = useState(false);

	const toggleTests = () => {
		setShowResume((prev) => !prev);
	};

	return (
		<>
			<div className="category">
				<p>{name}</p>
				<button className="categoryToggle" onClick={toggleTests}>
					{showResume ? 'Hide' : 'Show'}
				</button>
			</div>

			{showResume && (
				<Resume tests={tests} />
			)}

		</>
	);
}

export default Category;
